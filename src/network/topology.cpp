#include "topology.hpp"

#include <stdexcept>
#include <utility>


NodeId Topology::add_node(unique_ptr<Node> node) {

    if (!node) {
        throw invalid_argument("Node cannot be null");
    }

    NodeId id = static_cast<NodeId>(nodes.size());
    node->id = id;

    nodes.push_back(move(node));

    return id;
}

LinkId Topology::add_link(NodeId src, NodeId dst,const LinkConfig& config) {
    if (src >= nodes.size() || dst >= nodes.size()) {
        throw out_of_range("Invalid node id");
    }

    LinkId id = static_cast<LinkId>(links.size());

    links.push_back(Link{src, dst, config});
    nodes[src]->links.push_back(id);

    return id;
}

pair<LinkId, LinkId> Topology::add_bidirectional_link(NodeId a, NodeId b, const LinkConfig& config) {
    LinkId ab = add_link(a, b, config);
    LinkId ba = add_link(b, a, config);

    return {ab, ba};
}

unique_ptr<Topology> make_fat_tree(int k, const LinkConfig& config) {
    if (k < 2 || k % 2 != 0) {
        throw invalid_argument("Invalid fat tree size");
    }

    auto t = make_unique<Topology>();
    const int half = k/2;
    vector<NodeId> core_nodes(half * half);

    for (NodeId& id : core_nodes) {
        id = t->add_node(make_unique<Node>(NodeType::CoreSwitch));
    }

    for (int i = 0; i < k; ++i) {
        vector<NodeId> aggr_nodes(half);

        for (int a = 0; a < half; ++a) {
            aggr_nodes[a] = t->add_node(make_unique<Node>(NodeType::AggregationSwitch));

            // aggr <-> a-th core group
            for (int c = 0; c < half; ++c) {
                t->add_bidirectional_link(aggr_nodes[a], core_nodes[a * half + c], config);
            }
        }

        for (int e = 0; e < half; ++e) {
            NodeId edge_id = t->add_node(make_unique<Node>(NodeType::EdgeSwitch));

            for (NodeId aggr_id : aggr_nodes) {
                t->add_bidirectional_link(edge_id, aggr_id, config);
            }

            for (int g = 0; g < half; ++g) {
                NodeId gpu_id = t->add_node(make_unique<Node>(NodeType::GPU));

                t->add_bidirectional_link(gpu_id, edge_id, config);
            }
        }
    }

    return t;
}