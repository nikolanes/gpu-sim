#pragma once
#include "event.hpp"
#include "../network/topology.hpp"
#include "../network/router.hpp"
#include "rdma.hpp"
#include <iostream>

enum class PacketType {
    Data,
    Ack
};

struct Packet {

    NodeId src;
    NodeId dst;
    uint32_t size_bytes;

    FlowId flow_id;
    uint64_t seq_num;

    PacketType type;

};

struct LinkState {
    queue<PacketId> buffer;
    uint64_t buffer_size{};
    bool busy{};

    uint64_t dropped_packets{};
    uint64_t delivered_packets{};
};

class Simulator {
public:
    explicit Simulator(Topology& topology);

    const SimTime now() const;

    //void schedule(Event event);
    void run();
    PacketId add_packet(Packet p);
    void enqueue_packet(LinkId link_id, PacketId packet_id);

    LinkState& link_state(LinkId id) {
        return link_states.at(id);
    }

    const uint64_t& delivered_packets_count() const {
        return delivered_packets;
    }

    FlowId get_flow_id() {
        return flow_count++;
    }

    RdmaEndpoint& endpoint(NodeId id);

private:
    Topology& topology;
    SimTime now_ns{};
    uint64_t next_sequence{};

    uint64_t delivered_packets{};

    vector<LinkState> link_states;
    vector<Packet> packets;

    Router router;

    priority_queue<Event, vector<Event>, EventCompare> event_heap;

    vector<optional<RdmaEndpoint>> endpoints;
    FlowId flow_count{};

    void handle_event(const Event& event);
    void start_transmission(LinkId link_id, PacketId packet_id);
    void handle_transmission_finished(const Event& event);
    void handle_packet_arrived(const Event& event);
    void add_event(Event event);

};
