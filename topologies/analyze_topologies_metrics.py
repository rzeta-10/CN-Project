import pandas as pd
import matplotlib.pyplot as plt

# Define column names
column_names = ["Topology", "PacketLoss(%)", "Latency(ms)", "Throughput(Mbps)", "PDR(%)"]

# Load metrics with specified column names
data = pd.read_csv("network_metrics.csv", names=column_names)

# Print column names to verify
print(data.columns)

# Define a consistent figure size
figsize = (12, 8)

# Define bar width for consistency
bar_width = 0.2

# Create a figure and a set of subplots
fig, ax1 = plt.subplots(figsize=figsize)

# Create a secondary y-axis
ax2 = ax1.twinx()

# Define x-axis positions
x = range(len(data["Topology"]))

# Plot Packet Loss
ax1.bar([p - bar_width for p in x], data["PacketLoss(%)"], color='red', alpha=0.7, width=bar_width, label="Packet Loss (%)")

# Plot Latency
ax1.bar(x, data["Latency(ms)"], color='blue', alpha=0.7, width=bar_width, label="Latency (ms)")

# Plot Throughput
ax1.bar([p + bar_width for p in x], data["Throughput(Mbps)"], color='green', alpha=0.7, width=bar_width, label="Throughput (Mbps)")

# Check if 'PDR(%)' column exists and plot PDR on secondary y-axis
if 'PDR(%)' in data.columns:
    ax2.plot(x, data["PDR(%)"], color='orange', marker='o', linestyle='-', linewidth=2, markersize=8, label="PDR (%)")
else:
    print("Column 'PDR(%)' not found in the data.")

# Set x-axis labels
ax1.set_xticks(x)
ax1.set_xticklabels(data["Topology"])
ax1.set_xlabel("Topology")
ax1.set_ylabel("Packet Loss (%), Latency (ms), Throughput (Mbps)")
ax2.set_ylabel("PDR (%)")
ax1.set_title("Comparison of Network Metrics Across Topologies")

# Add legends for both axes
ax1.legend(loc="upper left")
ax2.legend(loc="upper right")

# Add gridlines
plt.grid(axis="y", linestyle="--", alpha=0.7)

# Show the merged plot
plt.show()