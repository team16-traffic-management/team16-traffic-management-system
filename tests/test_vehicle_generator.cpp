#include "../vehicle/VehicleGenerator.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void testProbabilityBoundaries() {
    VehicleGenerator generator({0.0, 1.0}, 7);
    for (std::uint64_t tick = 0; tick < 100; ++tick) {
        const std::vector<Vehicle> vehicles = generator.generateForTick(tick);
        require(vehicles.size() == 1, "zero and one probabilities are deterministic");
        require(vehicles[0].lane == 1, "only the lane with probability one generates");
        require(vehicles[0].tick == tick, "generated vehicle stores its tick");
    }
}

void testEmpiricalRatesAndLaneIndependence() {
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
    require(std::fabs(firstObserved - firstRate) < 0.02,
            "first lane empirical rate is close to configuration");
    require(std::fabs(secondObserved - secondRate) < 0.02,
            "second lane empirical rate is close to configuration");
    require(firstObserved < secondObserved,
            "lanes retain their different configured rates");
}

void testDynamicConfiguration() {
    VehicleGenerator generator({0.0, 0.0}, 99);
    generator.setArrivalRate(0, 1.0);
    generator.setArrivalRates({1.0, 0.0});
    const std::vector<Vehicle> vehicles = generator.generateForTick(42);
    require(vehicles.size() == 1 && vehicles[0].lane == 0,
            "updated rates take effect on the next tick");
    require(generator.getArrivalRate(0) == 1.0, "updated rate can be read");
}

void testInvalidConfiguration() {
    bool rejected = false;
    try {
        VehicleGenerator generator({0.2, 1.2}, 1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected, "probabilities above one are rejected");

    rejected = false;
    try {
        VehicleGenerator generator({0.2}, 1);
        generator.setArrivalRate(0, -0.1);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected, "negative probabilities are rejected");

    rejected = false;
    try {
        VehicleGenerator generator({0.2}, 1);
        generator.setArrivalRate(2, 0.5);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    require(rejected, "unknown lane indexes are rejected");
}
}

int main() {
    testProbabilityBoundaries();
    testEmpiricalRatesAndLaneIndependence();
    testDynamicConfiguration();
    testInvalidConfiguration();
    std::cout << "All VehicleGenerator tests passed." << std::endl;
    return EXIT_SUCCESS;
}
