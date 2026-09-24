#include "simulator.hpp"

Simulator::Simulator(Topology& topology)
    : topology(topology) {}

void Simulator::schedule(Event event) {
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

void Simulator::handle_event(const Event& event) {
    std::cout << "time=" << now_ns << " type=" << static_cast<int>(event.type) << " target=" << event.target_id << " packet=" << event.packet_id << '\n';
}
