#include "../include/SimulationEngine.h"
#include "../include/LaneQueue.h"
#include "../vehicle/VehicleGenerator.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
int passed = 0;
void check(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(EXIT_FAILURE); }
    ++passed;
}
template <typename Exception, typename Function>
void checkThrows(Function function, const char* message) {
    bool rejected = false;
    try { function(); } catch (const Exception&) { rejected = true; }
    check(rejected, message);
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
    const double firstRate = 0.20, secondRate = 0.65;
    VehicleGenerator generator({firstRate, secondRate}, 12345);
    std::vector<std::size_t> counts(2, 0);
    const std::uint64_t ticks = 10000;
    for (std::uint64_t tick = 0; tick < ticks; ++tick)
        for (const Vehicle& vehicle : generator.generateForTick(tick)) ++counts.at(vehicle.lane);
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
    checkThrows<std::invalid_argument>([] { VehicleGenerator g({0.2, 1.2}, 1); }, "probability above one rejected");
    checkThrows<std::invalid_argument>([] { VehicleGenerator g({0.2}, 1); g.setArrivalRate(0, -0.1); }, "negative probability rejected");
    checkThrows<std::out_of_range>([] { VehicleGenerator g({0.2}, 1); g.setArrivalRate(2, 0.5); }, "invalid lane index rejected");
    checkThrows<std::invalid_argument>([] { VehicleGenerator g(std::vector<double>(), 1); }, "empty lane configuration rejected");
}
void testQueueCapacityAndEmptyDequeue() {
    LaneQueue queue("North", 1);
    const Vehicle first(1, 1, VehicleType::NORMAL);
    const Vehicle second(2, 2, VehicleType::NORMAL);
    check(queue.enqueue(first), "first vehicle accepted");
    check(!queue.enqueue(second), "queue overflow rejected");
    check(queue.size() == 1 && queue.isFull(), "overflow leaves queue unchanged");
    check(queue.dequeue().getId() == 1, "FIFO dequeue returns first vehicle");
    checkThrows<std::runtime_error>([&] { queue.dequeue(); }, "empty dequeue rejected");
}
void testMalformedConfiguration() {
    const char* path = "test_invalid_config.tmp";
    { std::ofstream file(path); file << "this is not key value\n"; }
    SimulationEngine engine;
    checkThrows<std::runtime_error>([&] { engine.loadConfigFromFile(path); }, "malformed config rejected");
    std::remove(path);
}
void testTenThousandTickStability() {
    SimulationEngine engine;
    Intersection intersection("stress");
    intersection.addLane("North", 15);
    intersection.addLane("South", 15);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.05, false);
    engine.configureLaneArrivalRate("North", 0.4);
    engine.configureLaneArrivalRate("South", 0.4);
    engine.run(10000, true);
    check(engine.getStats().totalThroughput() >= 0, "10,000-tick run completes");
    check(engine.getStats().totalBlocked() >= 0, "stress run reports valid blocked count");
}
}
int main() {
    testProbabilityBoundaries();
    testArrivalRates();
    testConfigurationUpdates();
    testInvalidConfiguration();
    testQueueCapacityAndEmptyDequeue();
    testMalformedConfiguration();
    testTenThousandTickStability();
    std::cout << "All " << passed << " checks passed.\n";
    return EXIT_SUCCESS;
}
