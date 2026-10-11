#pragma once
#include <cstdint>
#include <deque>
#include <optional>
#include <vector>

#include "network/topology.hpp"
#include "event.hpp"
#include "network/router.hpp"

class Simulator;
using QpId = uint32_t;

struct RdmaConfig {
    uint64_t read_bytes_ps, write_bytes_ps;
    SimTime read_latency_ns, write_latency_ns;
    uint64_t tx_capacity_bytes, rx_capacity_bytes;
    uint32_t packet_bytes, window_packets, max_retries;
    SimTime timeout_ns;
};

struct SendRequest {
    FlowId flow;
    uint64_t bytes;
};

struct ReceiveRequest {
    uint64_t id;
    uint64_t bytes;
};

struct QueuePair {
    NodeId peer_node;
    QpId peer_qp;
    deque<SendRequest> sq;
    deque<ReceiveRequest> rq;
    optional<SendRequest> active_send;
    optional<ReceiveRequest> active_recv;
    
    uint64_t recv_total = 0, recv_written = 0;
};

class RdmaEndpoint {
public:

    RdmaEndpoint(NodeId node, const RdmaConfig& config);
    QpId create_qp(NodeId peer_node, QpId peer_qp);
    FlowId post_send(QpId qp, uint64_t bytes, Simulator& sim);
    void post_recv(QpId qp, ReceiveRequest request, Simulator& sim);
    void on_packet(PacketId packet_id, Simulator& sim);


private:
    NodeId node;
    RdmaConfig config;
    vector<QueuePair> qps;
    size_t next_qp = 0;

    void progress(Simulator& sim); 
    void handle_data(PacketId packet, Simulator& sim);
    void handle_ack(PacketId packet, Simulator& sim);
    void queue_ack(QpId qp, Simulator& sim);
};