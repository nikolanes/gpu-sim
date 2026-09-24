#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include <queue>


using SimTime = uint64_t;  // ns
using PacketId = uint32_t;

enum class EventType {
    TransmissionFinished,
    PacketArrived
};

struct Event {
    SimTime time_ns;
    uint64_t sequence;

    EventType type;
    uint32_t target_id;
    PacketId packet_id;
};

struct EventCompare {
    bool operator()(const Event& a, const Event& b) const {

        if (a.time_ns != b.time_ns)
            return a.time_ns > b.time_ns;
        if (a.type != b.type)
            return a.type > b.type;

        return a.sequence > b.sequence;
    }
};
