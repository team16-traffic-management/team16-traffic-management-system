#include "../include/SimulationEngine.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace {
constexpr int kTicksPerRun = 10000;
constexpr int kMeasuredRuns = 5;
constexpr double kTargetTicksPerSecond = 500.0;

double runOnce() {
    SimulationEngine engine;

    const std::vector<std::string> laneNames = {
        "North", "South", "East", "West"
    };

    // Exercise the engine with four intersections and four lanes per intersection.
    for (int i = 0; i < 4; ++i) {
        Intersection intersection("Junction-" + std::to_string(i + 1));
        for (const auto& laneName : laneNames) {
            intersection.addLane(laneName, 15);
        }
        engine.addIntersection(intersection);
    }

    engine.loadConfig(0.4, 0.05, false);

    const auto start = std::chrono::steady_clock::now();
    for (int tick = 0; tick < kTicksPerRun; ++tick) {
        engine.tick(true); // Suppress console rendering; measure simulation work.
    }
    const auto finish = std::chrono::steady_clock::now();

    const std::chrono::duration<double> elapsed = finish - start;
    if (elapsed.count() <= 0.0) {
        return 0.0;
    }

    return static_cast<double>(kTicksPerRun) / elapsed.count();
}
} // namespace

int main() {
    std::cout << "Traffic Management System performance benchmark\n"
              << "Configuration: 4 intersections, 4 lanes/intersection, "
              << "10,000 ticks/run\n"
              << "Arrival rate: 0.4 | Emergency probability: 0.05\n"
              << "Console rendering: disabled\n\n";

    std::cout << "Warm-up run...\n";
    (void)runOnce();

    std::vector<double> results;
    results.reserve(kMeasuredRuns);

    std::cout << std::fixed << std::setprecision(2);
    for (int run = 1; run <= kMeasuredRuns; ++run) {
        const double rate = runOnce();
        results.push_back(rate);
        std::cout << "Run " << run << ": " << rate << " ticks/second\n";
    }

    std::sort(results.begin(), results.end());
    const double median = results[results.size() / 2];
    const double average =
        std::accumulate(results.begin(), results.end(), 0.0) /
        static_cast<double>(results.size());

    std::cout << "\nAverage: " << average << " ticks/second\n"
              << "Median:  " << median << " ticks/second\n"
              << "SRS target: " << kTargetTicksPerSecond << " ticks/second\n";

    if (median >= kTargetTicksPerSecond) {
        std::cout << "RESULT: TARGET MET (based on median run)\n";
    } else {
        std::cout << "RESULT: TARGET NOT MET (based on median run)\n";
    }

    std::cout << "\nRecord the CPU, RAM, OS, compiler version, and these results "
                 "with the submission evidence.\n";
    return 0;
}
