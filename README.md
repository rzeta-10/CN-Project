# CN-Project - Simulation and Analysis of Networks using NS-3

## Overview
This project simulates and analyzes various network topologies and routing protocols in Mobile Ad Hoc Networks (MANETs) using the NS-3 simulator. The study focuses on both static and dynamic network scenarios, including mobility models for 3D space.

---

### Features
- Simulation of Star, Mesh, and Ring topologies.
- Implementation and comparison of MANET routing protocols: AODV, OLSR, DSDV, and DSR.
- Enhanced traffic simulation with both UDP and TCP flows.
- 3D mobility modeling using the Gauss-Markov Mobility Model.
- Metrics evaluation including throughput, latency, packet delivery ratio (PDR), and packet loss.

---

## Requirements
- **NS-3 Simulator**: Version 3.35 or above.
- **NetAnim**: For visualization of node movement and traffic flows.
- **Python**: For processing results (if needed for plotting).
- **C++ Compiler**: GCC version 9 or above.

---

## Running the Project

1. **Clone this Repository**:
   ```bash
   git clone https://github.com/rzeta-10/CN-Project.git
   ```

2. Move the **`.cc`** Files : Move the .cc files in each subdir to your NS-3 scratch directory

3. **Run the Simulation**: Use the following command to run a specific simulation:
    ```bash
    ./ns3 run scratch/<filename>.cc
    ```

4. **Visualization Using NetAnim**:
    ```bash
    export QT_QPA_PLATFORM=wayland  
    ./NetAnim
    ```

    Choose the **`.xml`** files present in the subdir
---

