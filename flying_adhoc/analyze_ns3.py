import os
import matplotlib.pyplot as plt
from scapy.all import rdpcap
import numpy as np

# Function to analyze PCAP file for throughput and latency
def analyze_pcap(pcap_file):
    packets = rdpcap(pcap_file)
    total_bytes = 0
    packet_count = 0
    start_time = None
    end_time = None

    for pkt in packets:
        if pkt.haslayer("IP"):
            packet_count += 1
            total_bytes += len(pkt)
            time = pkt.time
            if start_time is None:
                start_time = time
            end_time = time

    duration = end_time - start_time if end_time and start_time else 1
    throughput = (total_bytes * 8) / (duration * 1e6)  # Mbps

    return {
        "throughput": throughput,
        "packet_count": packet_count,
        "duration": duration,
    }


# Function to analyze Ascii Trace file for Packet Delivery Ratio (PDR)
def analyze_ascii(trace_file):
    sent = 0
    received = 0
    with open(trace_file, "r") as file:
        for line in file:
            if "+ " in line:  # Packet sent
                sent += 1
            elif "- " in line:  # Packet received
                received += 1

    # Ensure PDR calculation is valid
    pdr = (received / sent) * 100 if sent > 0 else 0
    return {
        "packets_sent": sent,
        "packets_received": received,
        "pdr": pdr
    }


# Normalize values between 0 and 1
def normalize(values):
    min_val = min(values)
    max_val = max(values)
    return [(v - min_val) / (max_val - min_val) if max_val != min_val else 0 for v in values]


# Combined plotting function for comparisons in one graph
def plot_combined(results):
    scenarios = list(results.keys())
    throughputs = [results[scenario]["throughput"] for scenario in scenarios]
    pdrs = [results[scenario]["pdr"] for scenario in scenarios]
    latencies = [results[scenario]["avg_latency"] for scenario in scenarios]

    # Normalize the values
    normalized_throughputs = normalize(throughputs)
    normalized_pdrs = normalize(pdrs)
    normalized_latencies = normalize(latencies)

    # Adjusting layout for better readability
    fig, ax = plt.subplots(figsize=(12, 8))

    width = 0.25  # Width of bars
    index = range(len(scenarios))

    # Plotting all metrics in one graph with normalization
    ax.bar(index, normalized_throughputs, width, label="Throughput (Mbps)", color="blue")
    ax.bar([i + width for i in index], normalized_pdrs, width, label="PDR (%)", color="green")
    ax.bar([i + 2 * width for i in index], normalized_latencies, width, label="Latency (s)", color="orange")

    # Shorten the names for better x-axis display
    ax.set_xlabel('Scenarios')
    ax.set_ylabel('Normalized Values')
    ax.set_title('Normalized Network Metrics Comparison')

    # Shortened x-tick labels
    ax.set_xticks([i + width for i in index])
    ax.set_xticklabels([scenario[:10] for scenario in scenarios])  # Only first 10 characters

    ax.legend()

    # Adjust layout for better space
    plt.tight_layout()
    plt.grid(True)
    plt.show()


# Main analysis function
def main():
    pcap_dir = "pcap_files"  # Directory containing PCAP files
    trace_file = "trace_files/Flying3D.tr"  # Path to the single Ascii Trace file

    # Ensure directory for PCAP files exists
    if not os.path.exists(pcap_dir):
        print("Error: Ensure the 'pcap_files' directory exists.")
        return

    # Gather all PCAP files
    pcap_files = sorted([f for f in os.listdir(pcap_dir) if f.endswith(".pcap")])

    if not pcap_files:
        print("Error: No PCAP files found.")
        return

    # Analyze all PCAP files and the single Trace file
    results = {}
    for pcap_file in pcap_files:
        scenario_name = os.path.splitext(pcap_file)[0]  # Use file name (without extension) as scenario name

        # Analyze PCAP
        pcap_metrics = analyze_pcap(os.path.join(pcap_dir, pcap_file))

        # Analyze Ascii Trace (only one file exists)
        ascii_metrics = analyze_ascii(trace_file)

        # Combine metrics
        results[scenario_name] = {
            "throughput": pcap_metrics["throughput"],
            "pdr": ascii_metrics["pdr"],
            "avg_latency": pcap_metrics["duration"] / pcap_metrics["packet_count"] if pcap_metrics["packet_count"] > 0 else 0,
        }

    # Plot comparisons
    plot_combined(results)


if __name__ == "__main__":
    main()
