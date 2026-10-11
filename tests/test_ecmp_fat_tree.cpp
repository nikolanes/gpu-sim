#include "sim/simulator.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <cassert>


int main() {


    const LinkConfig config{1'000'000'000ULL,
                            20'000,
                            1'000'000};

    const auto topology = make_fat_tree(4, config);

    vector<NodeId> gpus;
    for (size_t i = 0; i < topology->node_count(); ++i) {
        if (topology->node(i).type == NodeType::GPU) gpus.push_back(i);
    }

    assert(gpus.size() >= 3);

    const NodeId a = gpus.at(0);
    const NodeId b = gpus.at(4);
    const LinkId al = topology->node(a).links.at(0);
    const LinkId bl = topology->node(b).links.at(0);

    Router router(*topology);

    constexpr FlowId flow_cnt = 64;

    Simulator sim(*topology);

    for (FlowId flow = 0; flow < flow_cnt; ++flow) {
        for (int i = 0; i < 2; ++i) {

            const PacketId packet = sim.add_packet({a, b, 1000, flow});

            sim.enqueue_packet(al, packet);
        }
    }

    sim.run();

    assert(sim.delivered_packets_count() == flow_cnt * 2);

    cout << "Test passed" << endl;
}