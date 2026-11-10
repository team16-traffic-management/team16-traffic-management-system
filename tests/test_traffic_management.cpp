#include "../include/SimulationEngine.h"
#include "../include/LaneQueue.h"
#include "../include/ConsoleView.h"
#include "../vehicle/VehicleGenerator.h"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <stdexcept>
#include <vector>

namespace {
int passed = 0;
void check(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << std::endl; std::exit(EXIT_FAILURE); }
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
    { std::ofstream file(path); file << "this is not key value" << std::endl; }
    SimulationEngine engine;
    checkThrows<std::runtime_error>([&] { engine.loadConfigFromFile(path); }, "malformed config rejected");
    std::remove(path);
}
void testConfigurationValidationBoundaries() {
    const char* path = "test_config_validation.tmp";
    const std::string validConfig =
        "total_ticks=10\n"
        "adaptive_mode=false\n"
        "queue_capacity=5\n";

    const auto rejectsConfig = [&](const std::string& config, const char* message) {
        {
            std::ofstream file(path);
            file << config;
        }
        SimulationEngine engine;
        checkThrows<std::exception>([&] { engine.loadConfigFromFile(path); }, message);
        std::remove(path);
    };

    rejectsConfig("adaptive_mode=false\nqueue_capacity=5\n",
                  "missing total_ticks rejected");
    rejectsConfig("total_ticks=-1\nadaptive_mode=false\nqueue_capacity=5\n",
                  "negative total_ticks rejected");
    rejectsConfig("total_ticks=10000001\nadaptive_mode=false\nqueue_capacity=5\n",
                  "total_ticks above supported limit rejected");
    rejectsConfig("total_ticks=10\nadaptive_mode=maybe\nqueue_capacity=5\n",
                  "invalid adaptive_mode rejected");
    rejectsConfig("total_ticks=10\nadaptive_mode=false\nqueue_capacity=0\n",
                  "zero queue capacity rejected");
    rejectsConfig("total_ticks=10\nadaptive_mode=false\nqueue_capacity=10001\n",
                  "queue capacity above supported limit rejected");
    rejectsConfig(validConfig + "arrival_rate=-0.1\n",
                  "negative arrival rate rejected");
    rejectsConfig(validConfig + "arrival_rate=1.1\n",
                  "arrival rate above one rejected");
    rejectsConfig(validConfig + "arrival_rate=nan\n",
                  "non-finite arrival rate rejected");
    rejectsConfig(validConfig + "emergency_probability=0.2junk\n",
                  "trailing characters in probability rejected");
    rejectsConfig(validConfig + "unknown_setting=1\n",
                  "unknown configuration key rejected");
    rejectsConfig(validConfig + "arrival_rate=0.2\narrival_rate=0.3\n",
                  "duplicate configuration key rejected");
    rejectsConfig(validConfig + "intersection.Junction.lanes=North,\n",
                  "empty lane in intersection configuration rejected");
    rejectsConfig(validConfig + "intersection.Junction.lanes=North\nconnection.Junction=Junction\n",
                  "self-connected intersection rejected");
    rejectsConfig(validConfig + "intersection.Junction.lanes=North\nconnection.Junction=Missing\n",
                  "connection to missing intersection rejected");
}

void testConfigDrivenIntersections() {
    const char* path = "test_multi_intersection.tmp";
    {
        std::ofstream file(path);
        file << "total_ticks=25" << std::endl
             << "arrival_rate=1.0" << std::endl
             << "emergency_probability=0.0" << std::endl
             << "adaptive_mode=false" << std::endl
             << "queue_capacity=12" << std::endl
             << "intersection.Central.lanes=North,South" << std::endl
             << "intersection.Riverside.lanes=North,South" << std::endl
             << "connection.Central=Riverside" << std::endl;
    }
    SimulationEngine engine;
    const int ticks = engine.loadConfigFromFile(path);
    check(ticks == 25, "configured run length is returned");
    check(engine.getIntersectionCount() == 2, "intersections are created from configuration");
    check(engine.getConfiguredQueueCapacity() == 12, "configured queue capacity is applied");
    engine.run(ticks, true);
    check(engine.getTotalTransfers() > 0, "configured connection transfers vehicles");
    std::remove(path);
}
void testConsoleStatusFields() {
    Intersection intersection("Console-Test");
    intersection.addLane("North", 5);
    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());
    ConsoleView view;
    view.renderTick(7, intersection.getId(), intersection.getLanes(),
                    intersection.getSignalController(), false);
    std::cout.rdbuf(previous);
    const std::string output = captured.str();
    check(output.find("Console-Test") != std::string::npos, "console identifies intersection");
    check(output.find("Tick") != std::string::npos, "console displays current tick");
    check(output.find("Signal") != std::string::npos, "console displays signal state");
    check(output.find("Queue") != std::string::npos, "console displays lane queue length");
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


void testManualStopProducesPartialSummary() {
    SimulationEngine engine;
    Intersection intersection("manual-stop");
    intersection.addLane("North", 15);
    intersection.addLane("South", 15);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.05, false);

    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());
    std::thread stopper([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        requestSimulationStop();
    });

    engine.run(0, true); // Open-ended run; the request interrupts it.
    stopper.join();
    std::cout.rdbuf(previous);

    const std::string output = captured.str();
    check(output.find("TMS SIMULATION REPORT (") != std::string::npos,
          "manual stop still prints the end-of-run report");
    check(output.find("user requested stop") != std::string::npos,
          "manual stop is identified in the run summary");
    check(output.find("0 ticks executed") == std::string::npos,
          "manual stop report reflects ticks actually completed");
}

void testSignalPhaseTransitions() {
    SignalController signals(2, 1, 1);
    signals.addLane("North");
    signals.addLane("South");
    check(signals.getCurrentPhase("North") == SignalPhase::GREEN, "first lane starts green");
    check(signals.getCurrentPhase("South") == SignalPhase::RED, "inactive lane starts red");
    signals.updateSignalState(2);
    check(signals.getCurrentPhase("North") == SignalPhase::YELLOW, "green transitions to yellow");
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("North") == SignalPhase::RED, "yellow transitions to red");
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("South") == SignalPhase::GREEN, "signal rotates to next lane");
    checkThrows<std::invalid_argument>([] { SignalController invalid(0, 1, 1); }, "non-positive signal duration rejected");
    checkThrows<std::invalid_argument>([] { SignalController s; s.addLane(""); }, "empty signal lane rejected");
    checkThrows<std::invalid_argument>([] { SignalController s; s.addLane("A"); s.addLane("A"); }, "duplicate signal lane rejected");
}
void testEmergencyPreemption() {
    SignalController signals(2, 1, 1);
    signals.addLane("North");
    signals.addLane("South");
    check(!signals.preempt("Unknown"), "unknown emergency lane rejected");
    check(signals.preempt("South"), "valid emergency request accepted");
    check(!signals.preempt("North"), "second pending request rejected");
    signals.updateSignalState(2);
    signals.updateSignalState(1);
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("South") == SignalPhase::GREEN, "emergency lane receives green after current cycle");
    check(!signals.preempt("South"), "request for already active lane rejected");
}
void testStatisticsCalculations() {
    StatisticsCollector stats;
    const Vehicle first(101, 2, VehicleType::NORMAL);
    const Vehicle second(102, 5, VehicleType::EMERGENCY);
    check(stats.averageWaitTime() == 0.0, "empty statistics have zero average wait");
    stats.recordDequeue("Central", "North", first, 5);
    stats.recordDequeue("Central", "South", second, 9);
    stats.recordBlocked("Central", "North", 6);
    stats.recordBlocked("Riverside", "South", 7);
    stats.recordPreemption("Central", "South", 8);
    stats.updateMaxQueueLength("Central", "North", 4);
    stats.updateMaxQueueLength("Central", "North", 2);
    std::ostringstream report;
    std::streambuf* previous = std::cout.rdbuf(report.rdbuf());
    stats.printEndOfRunReport(10);
    std::cout.rdbuf(previous);
    check(report.str().find("4           ") != std::string::npos,
          "maximum queue length retains the highest observed value");
    check(stats.totalThroughput() == 2, "throughput counts dequeued vehicles");
    check(std::fabs(stats.averageWaitTime() - 3.5) < 1e-9, "average wait time is correct");
    check(stats.totalBlocked() == 2, "blocked events counted across intersections");
    check(stats.totalPreemptions() == 1, "preemption events counted");
    checkThrows<std::invalid_argument>([&] { stats.recordDequeue("Central", "North", first, 1); }, "dequeue before arrival rejected");
    checkThrows<std::invalid_argument>([&] { stats.recordBlocked("Central", "North", -1); }, "negative blocked-event tick rejected");
    checkThrows<std::invalid_argument>([&] { stats.recordPreemption("Central", "South", -1); }, "negative preemption tick rejected");
}
void testIntersectionValidationAndSignalRegistration() {
    checkThrows<std::invalid_argument>([] { Intersection invalid(""); }, "empty intersection ID rejected");
    Intersection intersection("Unit-Test");
    intersection.addLane("North", 3);
    check(intersection.getId() == "Unit-Test", "intersection ID retained");
    check(intersection.getLanes().size() == 1, "intersection stores added lane");
    check(intersection.getLanes()[0].capacity() == 3, "lane capacity retained");
    check(intersection.getSignalController().getCurrentPhase("North") == SignalPhase::GREEN, "added lane registered with signal controller");
    checkThrows<std::invalid_argument>([&] { intersection.addLane("North", 3); }, "duplicate intersection lane rejected");
    checkThrows<std::invalid_argument>([&] { intersection.addLane("South", 0); }, "zero lane capacity rejected");
    checkThrows<std::invalid_argument>([&] { intersection.addLane("", 3); }, "empty intersection lane rejected");
}


void testNoSimultaneousGreenSignals() {
    SignalController signals(2, 1, 1);
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};

    for (const std::string& lane : lanes) {
        signals.addLane(lane);
    }

    // Check the initial state and every subsequent tick across multiple cycles.
    for (int tick = 0; tick < 80; ++tick) {
        int greenCount = 0;
        for (const std::string& lane : lanes) {
            if (signals.getCurrentPhase(lane) == SignalPhase::GREEN) {
                ++greenCount;
            }
        }
        check(greenCount <= 1,
              "no more than one lane has a green signal at any tick");
        signals.updateSignalState(1);
    }
}

}
int main() {
    testProbabilityBoundaries();
    testArrivalRates();
    testConfigurationUpdates();
    testInvalidConfiguration();
    testQueueCapacityAndEmptyDequeue();
    testMalformedConfiguration();
    testConfigurationValidationBoundaries();
    testConfigDrivenIntersections();
    testConsoleStatusFields();
    testTenThousandTickStability();
    testManualStopProducesPartialSummary();
    testSignalPhaseTransitions();
    testNoSimultaneousGreenSignals();
    testEmergencyPreemption();
    testStatisticsCalculations();
    testIntersectionValidationAndSignalRegistration();
    std::cout << "All " << passed << " checks passed." << std::endl;
    return EXIT_SUCCESS;
}
