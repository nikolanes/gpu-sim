#include "simulator.hpp"

Simulator::Simulator(Topology& topology)
    : topology(topology), 
      link_states(topology.link_count()), 
      router(topology), 
      endpoints(topology.node_count()) {

        for (size_t id = 0; id < topology.node_count(); ++id) {

            if (topology.node(id).type == NodeType::GPU) {
                //endpoints.at(id).emplace(id);
            }
        }
    }


RdmaEndpoint& Simulator::endpoint(NodeId id) {
    auto& entry = endpoints.at(id);

    if (!entry.has_value()) {
        throw runtime_error("Invalid RDMA endpoint");
    }

    return entry.value();
}    

void Simulator::add_event(Event event) {
    if (event.time_ns < now_ns) {
        throw invalid_argument("Event cannot be before now");
    }

    event.sequence = next_sequence++;
    event_heap.push(event);
}

void Simulator::run() {
    while (!event_heap.empty()) {
        Event event = event_heap.top();
        event_heap.pop();

        now_ns = event.time_ns;
        handle_event(event);
    }
}

void Simulator::enqueue_packet(LinkId link_id, PacketId packet_id) {

    if (packet_id >= packets.size()) {
        throw invalid_argument("Invalid packet id");
    }
    if (link_id >= topology.link_count()) {
        throw invalid_argument("Invalid link id");
    }

    auto& state = link_states.at(link_id);
    if (state.busy) {

        const auto& link = topology.link(link_id);
        const auto& packet = packets.at(packet_id);
        if (link.config.buffer_capacity_bytes < state.buffer_size + packet.size_bytes) {
            state.dropped_packets++;
        }
        else {
            state.buffer.push(packet_id);
            state.buffer_size += packet.size_bytes;
        }

    }

    else {
        start_transmission(link_id, packet_id);
    }


}

void Simulator::start_transmission(LinkId link_id, PacketId packet_id) {

    if (packet_id >= packets.size()) {
        throw invalid_argument("Invalid packet id");
    }
    if (link_id >= topology.link_count()) {
        throw invalid_argument("Invalid link id");
    }

    auto& state = link_states.at(link_id);

    if (state.busy) {
        throw logic_error("Link is already busy");
    }

    auto& link = topology.link(link_id);
    auto& packet = packets.at(packet_id);

    uint64_t time{};
    auto bw = link.config.bandwidth_bps;

    if (bw == 0) {
        throw invalid_argument("Bandwidth cannot be 0");
    } else { 
        uint64_t bits = packet.size_bytes * 8ULL;
        uint64_t nbits = bits * 1'000'000'000ULL;

        time = (nbits / bw) + (nbits % bw != 0 ? 1 : 0);
    }

    if (now_ns > UINT64_MAX - time) {
        throw overflow_error("Time overflow");
    }

    add_event({now_ns + time, 
            0, 
            EventType::TransmissionFinished, 
            link_id, 
            packet_id});

    state.busy = true;



}
void Simulator::handle_transmission_finished(const Event& event) {
    const LinkId link_id = event.target_id;
    const auto& link = topology.link(link_id);
    auto& state = link_states.at(link_id);

    if (link.config.propagation_ns > UINT64_MAX - now_ns) {
        throw overflow_error("time overflow");
    }

    add_event({now_ns + link.config.propagation_ns, 
            0, 
            EventType::PacketArrived, 
            link.dst, event.packet_id});
    

    state.busy = false;

    if (!state.buffer.empty()) {
        const PacketId next = state.buffer.front();
        state.buffer.pop();

        state.buffer_size -= packets.at(next).size_bytes;
        start_transmission(link_id, next);
    }

    state.delivered_packets++;

    cout << "packet id " << event.packet_id << 
    " finished transmission at time " << event.time_ns << endl;

}

void Simulator::handle_packet_arrived(const Event& event) {

    const auto& packet_id = event.packet_id;
    const auto& packet = packets.at(packet_id);

    const NodeId target_id = event.target_id;

    if (target_id == packet.dst) {

        cout << "packet id " << packet_id << 
    " arrived at time " << event.time_ns << " dest node " << packet.dst << endl;


        delivered_packets++;
        //endpoint(target_id).on_packet(packet_id, *this);
        return;
    }

    const LinkId next = router.next_hop(target_id, packet.dst, packet.flow_id);

    cout << "packet id " << packet_id << " forwarded via link " << next << endl;

    enqueue_packet(next, packet_id);
}

void Simulator::handle_event(const Event& event) {
    switch (event.type) {
        case EventType::TransmissionFinished: {
            handle_transmission_finished(event);
            break;
        }
        case EventType::PacketArrived: {
            handle_packet_arrived(event);
            break;
        }
    }
}


PacketId Simulator::add_packet(Packet packet) {
    PacketId id = static_cast<PacketId>(packets.size());
    packets.push_back(packet);
    return id;
}
