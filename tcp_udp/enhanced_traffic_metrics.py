import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV data into a DataFrame
file_name = "enhanced_traffic_metrics.csv"  # Replace with the actual file path if necessary
data = pd.read_csv(file_name)

# Separate the data for UDP/CBR and TCP
udp_data = data[data["Flow Type"] == "UDP/CBR"]
tcp_data = data[data["Flow Type"] == "TCP"]

# Plotting everything on one graph
plt.figure(figsize=(12, 8))

# Plot Throughput
plt.plot(udp_data["Throughput (Mbps)"].reset_index(drop=True), label="UDP/CBR Throughput", marker="o", linestyle='-', color='b')
plt.plot(tcp_data["Throughput (Mbps)"].reset_index(drop=True), label="TCP Throughput", marker="o", linestyle='--', color='g')

# Plot Latency
plt.plot(udp_data["Latency (ms)"].reset_index(drop=True), label="UDP/CBR Latency", marker="^", linestyle='-', color='r')
plt.plot(tcp_data["Latency (ms)"].reset_index(drop=True), label="TCP Latency", marker="^", linestyle='--', color='orange')

# Plot Packet Loss
plt.plot(udp_data["Packet Loss (%)"].reset_index(drop=True), label="UDP/CBR Packet Loss", marker="s", linestyle='-', color='m')
plt.plot(tcp_data["Packet Loss (%)"].reset_index(drop=True), label="TCP Packet Loss", marker="s", linestyle='--', color='c')

# Graph settings
plt.title("Comparison of Throughput, Latency, and Packet Loss")
plt.xlabel("Sample")
plt.ylabel("Values (Mixed Units)")
plt.legend()
plt.grid()
plt.show()
