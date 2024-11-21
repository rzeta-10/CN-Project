#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include <fstream>
#include <sstream>

using namespace ns3;

// Function to calculate throughput
double CalculateThroughput(Ptr<PacketSink> sink, double duration) {
    uint64_t totalBytes = sink->GetTotalRx();
    return (totalBytes * 8) / (duration * 1000000.0); // Mbps
}

// Function to calculate packet delivery ratio
double CalculatePDR(uint64_t packetsSent, uint64_t packetsReceived) {
    return (packetsReceived * 100.0) / packetsSent;
}

// Save metrics to CSV
void SaveMetricsToCsv(std::string topology, double packetLoss, double latency, double throughput, double pdr) {
    std::ofstream outFile;
    outFile.open("network_metrics.csv", std::ios_base::app);
    if (outFile.is_open()) {
        outFile << topology << "," << packetLoss << "," << latency << "," << throughput << "," << pdr << "\n";
        outFile.close();
    }
}

void SimulateStarTopology(uint32_t &subnetCounter) {
    NS_LOG_UNCOND("Simulating Star Topology...");
    NodeContainer nodes;
    nodes.Create(10); // 1 central + 9 peripheral nodes

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // Add packet loss with ErrorModel
    Ptr<RateErrorModel> errorModel = CreateObject<RateErrorModel>();
    errorModel->SetAttribute("ErrorRate", DoubleValue(0.01)); // 1% packet loss

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;

    uint64_t totalPacketsSent = 0, totalPacketsReceived = 0;

    for (uint32_t i = 1; i < nodes.GetN(); i++) {
        devices = p2p.Install(NodeContainer(nodes.Get(0), nodes.Get(i)));

        // Apply the error model
        devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));

        // Use unique subnets for each link
        std::ostringstream subnet;
        subnet << "10." << subnetCounter++ << ".0.0";
        address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
        interfaces.Add(address.Assign(devices));
    }

    // Install Applications
    uint16_t port = 9;
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(0), port));
    onoff.SetConstantRate(DataRate("5Mbps"));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer app = onoff.Install(nodes.Get(1)); // Sending from Node 1
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(nodes.Get(0)); // Receiving at Node 0
    sinkApp.Start(Seconds(1.0));
    sinkApp.Stop(Seconds(10.0));

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    // Calculate Metrics
    Ptr<PacketSink> sinkPtr = sinkApp.Get(0)->GetObject<PacketSink>();
    double throughput = CalculateThroughput(sinkPtr, 9.0);
    double latency = 2.0;  // Fixed delay in ms (based on channel delay)
    totalPacketsReceived = sinkPtr->GetTotalRx() / 1024; // Received packets
    totalPacketsSent = 9 * 1024; // Approximation of packets sent in 9 seconds

    double pdr = CalculatePDR(totalPacketsSent, totalPacketsReceived);
    double packetLoss = 100.0 - pdr;

    SaveMetricsToCsv("Star", packetLoss, latency, throughput, pdr);
    Simulator::Destroy();
}

void SimulateMeshTopology(uint32_t &subnetCounter) {
    NS_LOG_UNCOND("Simulating Mesh Topology...");
    NodeContainer nodes;
    nodes.Create(7); // Fully connected mesh of 7 nodes

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("5ms"));

    Ptr<RateErrorModel> errorModel = CreateObject<RateErrorModel>();
    errorModel->SetAttribute("ErrorRate", DoubleValue(0.02)); // 2% packet loss

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;

    uint64_t totalPacketsSent = 0, totalPacketsReceived = 0;

    for (uint32_t i = 0; i < nodes.GetN(); i++) {
        for (uint32_t j = i + 1; j < nodes.GetN(); j++) {
            devices = p2p.Install(NodeContainer(nodes.Get(i), nodes.Get(j)));

            // Apply error model
            devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));

            // Use unique subnets for each link
            std::ostringstream subnet;
            subnet << "10." << subnetCounter++ << ".0.0";
            address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
            interfaces.Add(address.Assign(devices));
        }
    }

    // Install Applications
    uint16_t port = 9;
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(0), port));
    onoff.SetConstantRate(DataRate("3Mbps"));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer app = onoff.Install(nodes.Get(1)); // Sending from Node 1
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(nodes.Get(0)); // Receiving at Node 0
    sinkApp.Start(Seconds(1.0));
    sinkApp.Stop(Seconds(10.0));

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    // Calculate Metrics
    Ptr<PacketSink> sinkPtr = sinkApp.Get(0)->GetObject<PacketSink>();
    double throughput = CalculateThroughput(sinkPtr, 9.0);
    double latency = 5.0;  // Fixed delay in ms
    totalPacketsReceived = sinkPtr->GetTotalRx() / 1024;
    totalPacketsSent = 9 * 1024;

    double pdr = CalculatePDR(totalPacketsSent, totalPacketsReceived);
    double packetLoss = 100.0 - pdr;

    SaveMetricsToCsv("Mesh", packetLoss, latency, throughput, pdr);
    Simulator::Destroy();
}

void SimulateRingTopology(uint32_t &subnetCounter) {
    NS_LOG_UNCOND("Simulating Ring Topology...");
    NodeContainer nodes;
    nodes.Create(6); // Ring of 6 nodes

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("7Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("3ms"));

    Ptr<RateErrorModel> errorModel = CreateObject<RateErrorModel>();
    errorModel->SetAttribute("ErrorRate", DoubleValue(0.015)); // 1.5% packet loss

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    NetDeviceContainer devices;
    Ipv4InterfaceContainer interfaces;

    uint64_t totalPacketsSent = 0, totalPacketsReceived = 0;

    for (uint32_t i = 0; i < nodes.GetN(); i++) {
        devices = p2p.Install(NodeContainer(nodes.Get(i), nodes.Get((i + 1) % nodes.GetN())));

        // Apply error model
        devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));

        // Use unique subnets for each link
        std::ostringstream subnet;
        subnet << "10." << subnetCounter++ << ".0.0";
        address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
        interfaces.Add(address.Assign(devices));
    }

    // Install Applications
    uint16_t port = 9;
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(0), port));
    onoff.SetConstantRate(DataRate("4Mbps"));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer app = onoff.Install(nodes.Get(1)); // Sending from Node 1
    app.Start(Seconds(1.0));
    app.Stop(Seconds(10.0));

    PacketSinkHelper sink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApp = sink.Install(nodes.Get(0)); // Receiving at Node 0
    sinkApp.Start(Seconds(1.0));
    sinkApp.Stop(Seconds(10.0));

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    // Calculate Metrics
    Ptr<PacketSink> sinkPtr = sinkApp.Get(0)->GetObject<PacketSink>();
    double throughput = CalculateThroughput(sinkPtr, 9.0);
    double latency = 3.0;  // Fixed delay in ms
    totalPacketsReceived = sinkPtr->GetTotalRx() / 1024;
    totalPacketsSent = 9 * 1024;

    double pdr = CalculatePDR(totalPacketsSent, totalPacketsReceived);
    double packetLoss = 100.0 - pdr;

    SaveMetricsToCsv("Ring", packetLoss, latency, throughput, pdr);
    Simulator::Destroy();
}

int main(int argc, char *argv[]) {
    CommandLine cmd;
    cmd.Parse(argc, argv);

    uint32_t subnetCounter = 1;

    // Simulate different topologies
    SimulateStarTopology(subnetCounter);
    SimulateMeshTopology(subnetCounter);
    SimulateRingTopology(subnetCounter);

    return 0;
}