#include "../vehicle/VehicleGenerator.h"
#include "../include/StatisticsCollector.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
int passed = 0;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
    ++passed;
}

void testStatisticsMetrics() {
    StatisticsCollector stats;
    const Vehicle one(1, 2), two(2, 5, VehicleType::EMERGENCY);
    stats.recordDequeue("I1", "North", one, 6);
    stats.recordDequeue("I1", "North", two, 9);
    check(stats.averageWaitTime() == 4.0, "average waiting time calculated from dequeue events");
    check(stats.totalThroughput() == 2, "throughput counts processed vehicles");
    stats.recordBlocked("I1", "North", 10);
    stats.recordBlocked("I1", "South", 11);
    check(stats.totalBlocked() == 2, "blocked vehicles counted");
    stats.recordPreemption("I1", "North", 12);
    check(stats.totalPreemptions() == 1, "preemption events counted");
    check(stats.getBlockedEvents().size() == 2 && stats.getBlockedEvents()[1].tick == 11,
          "blocked timestamps are retained");
    check(stats.getPreemptionEvents().size() == 1 && stats.getPreemptionEvents()[0].tick == 12,
          "preemption timestamps are retained");
}
void testEmptyStatistics() {
    StatisticsCollector stats;
    check(stats.averageWaitTime() == 0.0, "empty average wait is zero");
    check(stats.totalThroughput() == 0 && stats.totalBlocked() == 0 && stats.totalPreemptions() == 0,
          "empty counters are zero");
}
void testProbabilityBoundaries() {
    VehicleGenerator generator({0.0, 1.0}, 7);
    for (std::uint64_t tick = 0; tick < 100; ++tick) {
        const std::vector<Vehicle> vehicles = generator.generateForTick(tick);
        check(vehicles.size() == 1, "probability 0 and 1 boundary");
        check(vehicles[0].lane == 1, "vehicle generated in correct lane");
        check(vehicles[0].tick == tick, "vehicle stores simulation tick");
    }
}

void testArrivalRates() {
    const double firstRate = 0.20;
    const double secondRate = 0.65;
    VehicleGenerator generator({firstRate, secondRate}, 12345);
    std::vector<std::size_t> counts(2, 0);
    const std::uint64_t ticks = 10000;

    for (std::uint64_t tick = 0; tick < ticks; ++tick) {
        for (const Vehicle& vehicle : generator.generateForTick(tick)) {
            ++counts[vehicle.lane];
        }
    }

    const double firstObserved = static_cast<double>(counts[0]) / ticks;
    const double secondObserved = static_cast<double>(counts[1]) / ticks;
    check(std::fabs(firstObserved - firstRate) < 0.02, "lane one arrival rate");
    check(std::fabs(secondObserved - secondRate) < 0.02, "lane two arrival rate");
    check(firstObserved < secondObserved, "lanes use independent configured rates");
}

void testConfigurationUpdates() {
    VehicleGenerator generator({0.0, 0.0}, 99);
    generator.setArrivalRate(0, 1.0);
    generator.setArrivalRates({1.0, 0.0});
    const std::vector<Vehicle> vehicles = generator.generateForTick(42);
    check(vehicles.size() == 1 && vehicles[0].lane == 0, "updated rate is applied");
    check(generator.getArrivalRate(0) == 1.0, "updated rate can be read");
    check(generator.getLaneCount() == 2, "lane count is reported");
}

void testInvalidConfiguration() {
    bool rejected = false;
    try {
        VehicleGenerator generator({0.2, 1.2}, 1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    check(rejected, "probability above one is rejected");

    rejected = false;
    try {
        VehicleGenerator generator({0.2}, 1);
        generator.setArrivalRate(0, -0.1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    check(rejected, "negative probability is rejected");

    rejected = false;
    try {
        VehicleGenerator generator({0.2}, 1);
        generator.setArrivalRate(2, 0.5);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    check(rejected, "invalid lane index is rejected");

    rejected = false;
    try {
        VehicleGenerator generator({}, 1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    check(rejected, "empty lane configuration is rejected");
}
}

int main() {
    testStatisticsMetrics();
    testEmptyStatistics();
    testProbabilityBoundaries();
    testArrivalRates();
    testConfigurationUpdates();
    testInvalidConfiguration();
    std::cout << "All " << passed << " checks passed.\n";
    return EXIT_SUCCESS;
}
