
#include "ConsoleView.h"

#include <iostream>
#include <string>

namespace {
    std::string signalToString(SignalColor color) {
        switch (color) {
            case SignalColor::GREEN:
                return "GREEN";

            case SignalColor::YELLOW:
                return "YELLOW";

            case SignalColor::RED:
                return "RED";
        }

        return "UNKNOWN";
    }
}

void ConsoleView::renderTick(
    int tick,
    const std::vector<LaneQueue>& lanes,
    const SignalController& signals
) const {
    std::cout << "\n========== TICK " << tick
              << " ==========\n";

    for (const LaneQueue& lane : lanes) {
        const int queueSize = lane.size();
        const int capacity = lane.capacity();

        std::cout << "Lane: " << lane.name()
                  << " | Signal: "
                  << signalToString(signals.getColor(lane.name()))
                  << " | Queue: " << queueSize
                  << "/" << capacity
                  << " | Vehicles: "
                  << std::string(
                         queueSize > 0 ? queueSize : 0,
                         '#'
                     )
                  << '\n';
    }

    std::cout << "========================\n";
}
