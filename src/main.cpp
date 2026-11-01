#include "../vehicle/VehicleGenerator.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <queue>
#include <vector>

int main() {
    const std::vector<double> arrivalRates = {0.65, 0.45, 0.30, 0.55};
    VehicleGenerator generator(arrivalRates, 2026);
    std::array<std::queue<Vehicle>, 4> laneQueues;
    std::array<std::size_t, 4> vehiclesPassed = {{0, 0, 0, 0}};
    const std::size_t simulationTicks = 20;
    const std::size_t greenLaneDuration = 3;

    std::cout << "TRAFFIC MANAGEMENT SYSTEM\n";
    std::cout << "Four-lane intersection | " << simulationTicks << " simulation ticks\n\n";

    for (std::size_t tick = 0; tick < simulationTicks; ++tick) {
        const std::vector<Vehicle> arrivals = generator.generateForTick(tick);
        for (const Vehicle& vehicle : arrivals) {
            laneQueues[vehicle.lane].push(vehicle);
        }

        const std::size_t greenLane = (tick / greenLaneDuration) % laneQueues.size();
        if (!laneQueues[greenLane].empty()) {
            laneQueues[greenLane].pop();
            ++vehiclesPassed[greenLane];
        }

        std::size_t waiting = 0;
        for (const auto& lane : laneQueues) {
            waiting += lane.size();
        }

        std::cout << "Tick " << tick + 1
                  << " | Green: Lane " << greenLane + 1
                  << " | Arrivals: " << arrivals.size()
                  << " | Waiting: " << waiting << '\n';
    }

    std::cout << "\nSIMULATION SUMMARY\n";
    std::size_t totalPassed = 0;
    std::size_t totalWaiting = 0;
    for (std::size_t lane = 0; lane < laneQueues.size(); ++lane) {
        std::cout << "Lane " << lane + 1
                  << " | Passed: " << vehiclesPassed[lane]
                  << " | Still waiting: " << laneQueues[lane].size() << '\n';
        totalPassed += vehiclesPassed[lane];
        totalWaiting += laneQueues[lane].size();
    }

    std::cout << "Total vehicles passed: " << totalPassed << '\n';
    std::cout << "Total vehicles waiting: " << totalWaiting << '\n';
    return 0;
}
