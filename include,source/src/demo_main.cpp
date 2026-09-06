// demo_main.cpp
// Standalone demo so you can see ConsoleView + StatisticsCollector working
// TODAY, using mock lanes/signals/vehicles instead of the real engine.
// This file is NOT part of the final integrated main.cpp — it's only here
// so your two modules are provably working before integration day.
#include <vector>
#include "ConsoleView.h"
#include "StatisticsCollector.h"
#include "LaneQueue.h"
#include "SignalController.h"

int main() {
    std::vector<LaneQueue> lanes = {
        LaneQueue("North", 8),
        LaneQueue("South", 8),
        LaneQueue("East",  8),
        LaneQueue("West",  8)
    };
    SignalController signals;
    ConsoleView view;
    StatisticsCollector stats;

    int nextVehicleId = 0;

    for (int tick = 0; tick < 10; ++tick) {
        // Mock signal logic: North/South green for 3 ticks, then East/West.
        bool nsGreen = (tick / 3) % 2 == 0;
        signals.mockSetColor("North", nsGreen ? SignalColor::GREEN : SignalColor::RED);
        signals.mockSetColor("South", nsGreen ? SignalColor::GREEN : SignalColor::RED);
        signals.mockSetColor("East",  nsGreen ? SignalColor::RED   : SignalColor::GREEN);
        signals.mockSetColor("West",  nsGreen ? SignalColor::RED   : SignalColor::GREEN);

        // Mock arrivals.
        if (tick % 2 == 0) lanes[0].mockPush({nextVehicleId++, tick, Vehicle::Type::NORMAL});
        if (tick % 3 == 0) lanes[2].mockPush({nextVehicleId++, tick, Vehicle::Type::NORMAL});

        // Mock dequeue: if a lane is Green and has cars, clear the front one.
        for (auto& lane : lanes) {
            if (signals.getColor(lane.name()) == SignalColor::GREEN && lane.size() > 0) {
                Vehicle v = lane.mockFront();
                lane.mockPop();
                stats.recordDequeue(lane.name(), v, tick);
            }
        }

        view.renderTick(tick, lanes, signals);
    }

    stats.printEndOfRunReport();
    return 0;
}
