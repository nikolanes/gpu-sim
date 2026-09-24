#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

using namespace std;

using NodeId = uint32_t;
using LinkId = uint32_t;

enum class NodeType {
    GPU,
    EdgeSwitch,
    AggregationSwitch,
    CoreSwitch
};

class Node {
public:
    NodeType type;
    NodeId id{};
    vector<LinkId> links;

    explicit Node(NodeType type) : type(type) {}

    Node(NodeType type, NodeId id) : type(type), id(id) {}

    virtual ~Node() = default;
};

struct LinkConfig {
    uint64_t bandwidth_bps;
    uint64_t propagation_ns;
    uint64_t buffer_capacity_bytes;
};

struct Link {
    NodeId src;
    NodeId dst;
    LinkConfig config;
};

class Topology {
private:
    vector<unique_ptr<Node>> nodes;
    vector<Link> links;

public:

    NodeId add_node(unique_ptr<Node> node);

    LinkId add_link(NodeId src, NodeId dst, const LinkConfig& config);

    pair<LinkId, LinkId> add_bidirectional_link(NodeId a, NodeId b, const LinkConfig& config);

    Node& node(NodeId id) {
        return *nodes.at(id);
    }

    const Node& node(NodeId id) const {
        return *nodes.at(id);
    }

    Link& link(LinkId id) {
        return links.at(id);
    }

    const Link& link(LinkId id) const {
        return links.at(id);
    }

    size_t node_count() const {
        return nodes.size();
    }

    size_t link_count() const {
        return links.size();
    }
};

unique_ptr<Topology> make_fat_tree(int k, const LinkConfig& config);