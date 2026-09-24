#pragma once
#include "event.hpp"
#include "../network/topology.hpp"
#include <iostream>


class Simulator {
public:
    explicit Simulator(Topology& topology);

    SimTime now() const;

    void schedule(Event event);
    void run();

private:
    Topology& topology;
    SimTime now_ns = 0;
    uint64_t next_sequence = 0;

    priority_queue<Event, vector<Event>, EventCompare> event_heap;

    void handle_event(const Event& event);

};