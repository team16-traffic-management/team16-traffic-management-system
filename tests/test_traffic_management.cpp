#include "../include/LaneQueue.h"
#include "../vehicle/VehicleGenerator.h"
#include "../include/TrafficSignalController.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
int passed = 0;
void check(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(EXIT_FAILURE); }
    ++passed;
}


void testSignalTransitions() {
    SignalController controller(2, 1, 2);
    controller.addLane("A"); controller.addLane("B");
    check(controller.getCurrentPhase("A") == SignalPhase::GREEN, "initial lane is green");
    controller.updateSignalState(2);
    check(controller.getCurrentPhase("A") == SignalPhase::YELLOW, "green transitions to yellow");
    controller.updateSignalState(1);
    check(controller.getCurrentPhase("A") == SignalPhase::RED, "yellow transitions to red");
    controller.updateSignalState(2);
    check(controller.getCurrentPhase("B") == SignalPhase::GREEN, "red transitions to next lane green");
}
void testLargeTickUpdates() {
    SignalController controller(2, 1, 2);
    controller.addLane("A"); controller.addLane("B");
    controller.updateSignalState(5);
    check(controller.getCurrentPhase("B") == SignalPhase::GREEN, "large delta processes multiple transitions");
    controller.updateSignalState(1);
    check(controller.getCurrentPhase("B") == SignalPhase::GREEN, "elapsed remainder is preserved");
}
void testConflictingGreenSignals() {
    SignalController controller;
    controller.addLane("A"); controller.addLane("B");
    check(controller.getCurrentPhase("A") == SignalPhase::GREEN, "one active lane is green");
    check(controller.getCurrentPhase("B") == SignalPhase::RED, "conflicting lane remains red");
}
void testAdaptiveTimingPreservesBase() {
    SignalController controller(6, 2, 4);
    controller.addLane("A"); controller.addLane("B");
    controller.setAdaptiveMode(true, 5);
    controller.updateAdaptiveTimers({{"A", 10}});
    controller.updateSignalState(11);
    check(controller.getCurrentPhase("A") == SignalPhase::YELLOW, "adaptive green duration extends configured base to 11 ticks");
    controller.updateSignalState(2); controller.updateSignalState(4);
    check(controller.getCurrentPhase("B") == SignalPhase::GREEN, "cycle advances after configured phases");
    controller.updateAdaptiveTimers({{"B", 0}});
    controller.updateSignalState(6);
    check(controller.getCurrentPhase("B") == SignalPhase::YELLOW, "low queue restores configured base green duration");
}
void testEmergencyPreemption() {
    SignalController controller(10, 3, 10);
    controller.addLane("A"); controller.addLane("B");
    check(controller.preempt("B"), "emergency request accepted");
    check(controller.getCurrentPhase("B") == SignalPhase::GREEN, "emergency lane receives immediate green");
    check(controller.getCurrentPhase("A") == SignalPhase::RED, "previous lane is immediately red");
    check(!controller.preempt("missing"), "unknown lane preemption rejected");
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
        for (const Vehicle& vehicle : generator.generateForTick(tick)) ++counts[vehicle.lane];
    const double firstObserved = static_cast<double>(counts[0]) / ticks;
    const double secondObserved = static_cast<double>(counts[1]) / ticks;
    check(std::fabs(firstObserved - firstRate) < 0.02, "lane one arrival rate");
    check(std::fabs(secondObserved - secondRate) < 0.02, "lane two arrival rate");
    check(firstObserved < secondObserved, "lanes use independent configured rates");
}
void testConfigurationUpdates() {
    VehicleGenerator generator({0.0, 0.0}, 99);
    generator.setArrivalRate(0, 1.0); generator.setArrivalRates({1.0, 0.0});
    const std::vector<Vehicle> vehicles = generator.generateForTick(42);
    check(vehicles.size() == 1 && vehicles[0].lane == 0, "updated rate is applied");
    check(generator.getArrivalRate(0) == 1.0, "updated rate can be read");
    check(generator.getLaneCount() == 2, "lane count is reported");
}
void testInvalidConfiguration() {
    bool rejected = false;
    try { VehicleGenerator generator({0.2, 1.2}, 1); } catch (const std::invalid_argument&) { rejected = true; }
    check(rejected, "probability above one is rejected");
    rejected = false;
    try { VehicleGenerator generator({0.2}, 1); generator.setArrivalRate(0, -0.1); } catch (const std::invalid_argument&) { rejected = true; }
    check(rejected, "negative probability is rejected");
    rejected = false;
    try { VehicleGenerator generator({0.2}, 1); generator.setArrivalRate(2, 0.5); } catch (const std::out_of_range&) { rejected = true; }
    check(rejected, "invalid lane index is rejected");
    rejected = false;
    try { VehicleGenerator generator({}, 1); } catch (const std::invalid_argument&) { rejected = true; }
    check(rejected, "empty lane configuration is rejected");
}
void testLaneQueueFifo() {
    LaneQueue queue("FIFO", 5);
    const Vehicle first(1, 0, 1), second(2, 0, 2), third(3, 0, 3);
    check(queue.enqueue(first) && queue.enqueue(second) && queue.enqueue(third), "normal vehicles enqueue");
    check(queue.dequeue().getId() == first.getId(), "FIFO returns first vehicle first");
    check(queue.dequeue().getId() == second.getId(), "FIFO returns second vehicle second");
    check(queue.dequeue().getId() == third.getId(), "FIFO returns third vehicle third");
}
void testEmergencyPriority() {
    LaneQueue queue("Emergency", 5);
    const Vehicle normal1(1, 0, 1), normal2(2, 0, 2);
    const Vehicle emergency1(3, 0, 3, VehicleType::EMERGENCY);
    const Vehicle emergency2(4, 0, 4, VehicleType::EMERGENCY);
    queue.enqueue(normal1); queue.enqueue(normal2); queue.enqueue(emergency1); queue.enqueue(emergency2);
    check(queue.dequeue().getId() == emergency1.getId(), "first emergency vehicle gets priority");
    check(queue.dequeue().getId() == emergency2.getId(), "emergency vehicles retain FIFO order");
    check(queue.dequeue().getId() == normal1.getId(), "normal FIFO order is preserved");
}
void testQueueCapacity() {
    LaneQueue queue("Capacity", 2);
    check(queue.enqueue(Vehicle(1, 0, 1)), "first vehicle fits");
    check(queue.enqueue(Vehicle(2, 0, 2)), "second vehicle fills capacity");
    check(!queue.enqueue(Vehicle(3, 0, 3)), "vehicle rejected at capacity");
    check(queue.size() == 2 && queue.isFull(), "queue remains at capacity");
}
void testEmptyDequeue() {
    LaneQueue queue("Empty", 2);
    bool rejected = false;
    try { (void)queue.dequeue(); } catch (const std::runtime_error&) { rejected = true; }
    check(rejected, "dequeue from empty queue throws");
}
}
int main() {
    // TrafficSignalController tests
    testSignalTransitions();
    testLargeTickUpdates();
    testConflictingGreenSignals();
    testAdaptiveTimingPreservesBase();
    testEmergencyPreemption();

    // Existing configuration and vehicle generation tests
    testProbabilityBoundaries();
    testArrivalRates();
    testConfigurationUpdates();
    testInvalidConfiguration();

    // LaneQueue tests
    testLaneQueueFifo();
    testEmergencyPriority();
    testQueueCapacity();
    testEmptyDequeue();
    std::cout << "All " << passed << " checks passed.\n";
    return EXIT_SUCCESS;
}
