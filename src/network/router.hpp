#pragma once

#include "topology.hpp"
#include <vector>

using namespace std;

using FlowId = uint64_t;

class Router {
public:
    explicit Router(const Topology& topology);

    const vector<LinkId>& next_hops(NodeId current, NodeId destination) const;
    LinkId next_hop(NodeId start, NodeId dest, FlowId flow_id) const;

private:
    vector<vector<vector<LinkId>>> hops;

    void build_hops_bfs(const Topology &topology, const vector<vector<LinkId>> &adj, const NodeId dest);
    void build_routes(const Topology &topology);
    uint64_t mix_hash(uint64_t key) const;

};