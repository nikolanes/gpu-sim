#include "sim/simulator.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <cassert>


int main() {
    Topology topology;

    auto add_node = [&](NodeType type) {
        return topology.add_node(make_unique<Node>(type));
    };

    const NodeId a = add_node(NodeType::GPU);
    const NodeId s = add_node(NodeType::EdgeSwitch);
    const NodeId x = add_node(NodeType::AggregationSwitch);
    const NodeId y = add_node(NodeType::AggregationSwitch);
    const NodeId b = add_node(NodeType::GPU);

    const LinkConfig config{1'000'000'000ULL,
                            20'000,
                            1'000'000};

    // a -> s -> x -> b
    // a -> s -> y -> b
    const LinkId as = topology.add_link(a, s, config);
    const LinkId sx = topology.add_link(s, x, config);
    const LinkId sy = topology.add_link(s, y, config);
    topology.add_link(x, b, config);
    topology.add_link(y, b, config);

    Router router(topology);

    constexpr FlowId flow_cnt = 64;
    bool usedx = false;
    bool usedy = false;

    for (FlowId flow = 0; flow < flow_cnt; ++flow) {

        const LinkId hop = router.next_hop(s, b, flow);

        router.next_hop(s, b, flow + 1);

        assert(router.next_hop(s, b, flow) == hop);

        usedx = (usedx || hop == sx);
        usedy = (usedy || hop == sy);
    }

    assert(usedx && usedy);

    Simulator sim(topology);

    for (FlowId flow = 0; flow < flow_cnt; ++flow) {
        for (int i = 0; i < 2; ++i) {

            const PacketId packet = sim.add_packet({a, b, 1000, flow});

            sim.enqueue_packet(as, packet);
        }
    }

    sim.run();

    assert(sim.delivered_packets_count() == flow_cnt * 2);

    cout << "Test passed" << endl;
}