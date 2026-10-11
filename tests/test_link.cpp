#include "sim/simulator.hpp"
#include <memory>

int main() {
    Topology topology;


    LinkConfig config{1'000'000'000ULL,
                    20'000,
                    1'000};

    NodeId a = topology.add_node(make_unique<Node>(NodeType::GPU));
    NodeId sw = topology.add_node(make_unique<Node>(NodeType::EdgeSwitch));
    NodeId b = topology.add_node(make_unique<Node>(NodeType::GPU));

LinkId first_link = topology.add_link(a, sw, config);
LinkId second_link = topology.add_link(sw, b, config);

    Simulator sim(topology);

    PacketId p0 = sim.add_packet({a, b, 1'000});
    PacketId p1 = sim.add_packet({a, b, 1'000});
    PacketId p2 = sim.add_packet({a, b, 1'000});

    sim.enqueue_packet(first_link, p0);
    sim.enqueue_packet(first_link, p1);
    sim.enqueue_packet(first_link, p2);

    sim.run();

    const auto& state1 = sim.link_state(first_link);
    const auto& state2 = sim.link_state(second_link);
    cout << "delivered packets " << state1.delivered_packets << 
    " dropped packets " << state1.dropped_packets << 
    " waiting bytes " << state1.buffer_size << 
    " busy " << state1.busy << endl;
}