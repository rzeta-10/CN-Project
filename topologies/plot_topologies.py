import matplotlib.pyplot as plt
import networkx as nx

def plot_topologies():
    fig, axs = plt.subplots(1, 3, figsize=(15, 5))

    # Star Topology
    star = nx.star_graph(9)  # 1 central node + 9 peripheral nodes
    pos_star = nx.spring_layout(star)
    nx.draw(star, pos_star, ax=axs[0], with_labels=True, node_size=500, node_color="lightblue", edge_color="gray")
    axs[0].set_title("Star Topology")

    # Mesh Topology
    mesh = nx.complete_graph(7)  # Fully connected mesh of 7 nodes
    pos_mesh = nx.spring_layout(mesh)
    nx.draw(mesh, pos_mesh, ax=axs[1], with_labels=True, node_size=500, node_color="lightgreen", edge_color="gray")
    axs[1].set_title("Mesh Topology")

    # Ring Topology
    ring = nx.cycle_graph(6)  # Circular ring of 6 nodes
    pos_ring = nx.circular_layout(ring)
    nx.draw(ring, pos_ring, ax=axs[2], with_labels=True, node_size=500, node_color="lightpink", edge_color="gray")
    axs[2].set_title("Ring Topology")

    plt.tight_layout()
    plt.show()

# Call the function to plot the topologies
plot_topologies()
