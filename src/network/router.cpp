#include "router.hpp"

#include <cstdint>
#include <queue>

Router::Router(const Topology &topology) {
    build_routes(topology);
}

const vector<LinkId>& Router::next_hops(NodeId start, NodeId dest) const {
    return hops.at(start).at(dest);
}

uint64_t Router::mix_hash(uint64_t key) const {

    key ^= (key >> 31);
    key *= 0x99bcf6822b23ca35ULL;
    key ^= (key >> 30);
    key *= 0x14020a57acced8b7ULL;
    key ^= (key >> 33);

    return key;
}


LinkId Router::next_hop(NodeId start, NodeId dest, FlowId flow_id) const {

    const auto& links = next_hops(start, dest);
    if (links.empty()) {
        throw runtime_error("Invalid start dest combination");
    }

    const uint64_t salt = mix_hash(start + 0x99bcf6822b23ca35ULL);
    const uint64_t hash = mix_hash(flow_id ^ salt);

    return links.at(hash % links.size());

}

/*vector<uint64_t> Router::bfs(const Topology &topology, const vector<vector<LinkId>> &adj, const NodeId start) const {

    vector<int64_t> dist(topology.node_count(), -1);
    queue<NodeId> q;

    dist.at(start) = 0;
    q.push(start);

    while (!q.empty()) {

        const NodeId v = q.front();
        q.pop();

        for (LinkId id : adj.at(v)) {
            const NodeId u = topology.link(id).src;

            if (dist[u] != -1) {
                continue;
            }
            dist[u] = dist[v] + 1;
            q.push(u);
        }
    }

    return dist;
}*/

void Router::build_hops_bfs(const Topology &topology, const vector<vector<LinkId>> &adj, const NodeId dest) {

    vector<int64_t> dist(topology.node_count(), -1);
    queue<NodeId> q;

    dist.at(dest) = 0;
    q.push(dest);

    while (!q.empty()) {
        const NodeId v = q.front();
        q.pop();

        for (LinkId id : adj.at(v)) {
            const NodeId u = topology.link(id).src;
            const auto need = dist[v] + 1;

            if (dist[u] == -1) {
                dist[u] = need;
                q.push(u);
            }

            if (dist[u] == need) {
                hops[u][dest].push_back(id);
            }
        }
    }
}

void Router::build_routes(const Topology &topology) {

    const auto n = topology.node_count();
    hops.assign(n, vector<vector<LinkId>>(n));

    vector<vector<LinkId>> adj(n);

    // build reverse adj matrix
    for (size_t i = 0; i < topology.link_count(); ++i) {
        const auto& link = topology.link(i);
        adj.at(link.dst).push_back(i);
    }

    for (size_t dest = 0; dest < n; ++dest) {

        if (topology.node(dest).type == NodeType::GPU) {
            build_hops_bfs(topology, adj, dest);
        }
    }
}