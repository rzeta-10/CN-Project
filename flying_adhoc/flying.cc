#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-helper.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Flying");

int main() {
    // Create nodes
    NodeContainer nodes;
    nodes.Create(20); // 20 wireless nodes

    // Configure WiFi
    WifiHelper wifi;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode", StringValue("OfdmRate54Mbps"));

    YansWifiPhyHelper wifiPhy;
    wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO); // Set data link type
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    wifiPhy.SetChannel(wifiChannel.Create());

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(wifiPhy, mac, nodes);

    // Enable AODV routing
    AodvHelper aodv;
    InternetStackHelper internet;
    internet.SetRoutingHelper(aodv);
    internet.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("192.168.0.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Configure mobility (3D Mobility)
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::GaussMarkovMobilityModel",
                              "Bounds", BoxValue(Box(0, 100, 0, 100, 0, 100)),
                              "TimeStep", TimeValue(Seconds(0.5)),
                              "Alpha", DoubleValue(0.85),
                              "MeanVelocity", StringValue("ns3::UniformRandomVariable[Min=1.0|Max=5.0]"),
                              "MeanDirection", StringValue("ns3::UniformRandomVariable[Min=0|Max=6.28]"),
                              "MeanPitch", StringValue("ns3::UniformRandomVariable[Min=0|Max=0.05]"));
    mobility.SetPositionAllocator("ns3::RandomBoxPositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=0|Max=100]"),
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=0|Max=100]"),
                                  "Z", StringValue("ns3::UniformRandomVariable[Min=0|Max=100]"));
    mobility.Install(nodes);

    // Set up applications
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(5));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Enable tracing
    wifiPhy.EnablePcapAll("Flying3D");
    AsciiTraceHelper ascii;
    wifiPhy.EnableAsciiAll(ascii.CreateFileStream("Flying3D.tr"));

    // NetAnim
    AnimationInterface anim("Flying3D.xml");

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
