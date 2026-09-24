#include "sim/simulator.hpp"

int main() {
    Topology topology;
    Simulator sim(topology);

    sim.schedule({30, 0, EventType::PacketArrived,        0, 1});
    sim.schedule({10, 0, EventType::PacketArrived,        0, 2});
    sim.schedule({10, 0, EventType::TransmissionFinished, 0, 3});
    sim.schedule({10, 0, EventType::PacketArrived,        0, 4});

    sim.run();
}