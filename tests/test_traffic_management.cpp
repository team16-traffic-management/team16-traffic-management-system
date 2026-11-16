#include "../include/SimulationEngine.h"
#include "../include/LaneQueue.h"
#include "../include/ConsoleView.h"
#include "../vehicle/VehicleGenerator.h"
#include <algorithm>
#include <chrono>
#include <csignal>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <set>
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
void testVehicleIdsAreUniqueAcrossLanesAndIntersections() {
    VehicleGenerator generator(1.0, 0.0, 12345);
    std::set<int> vehicleIds;

    for (int tick = 0; tick < 50; ++tick) {
        const Vehicle centralNorth =
            generator.generateVehicle("Central", "North", tick);
        const Vehicle centralSouth =
            generator.generateVehicle("Central", "South", tick);
        const Vehicle riversideNorth =
            generator.generateVehicle("Riverside", "North", tick);
        const Vehicle riversideSouth =
            generator.generateVehicle("Riverside", "South", tick);

        check(vehicleIds.insert(centralNorth.getId()).second,
              "vehicle ID is unique for Central North");
        check(vehicleIds.insert(centralSouth.getId()).second,
              "vehicle ID is unique for Central South");
        check(vehicleIds.insert(riversideNorth.getId()).second,
              "vehicle ID is unique for Riverside North");
        check(vehicleIds.insert(riversideSouth.getId()).second,
              "vehicle ID is unique for Riverside South");
    }

    check(vehicleIds.size() == 200,
          "all generated vehicles across lanes and intersections have distinct IDs");
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
void testEmergencyQueuePriorityAndStableOrdering() {
    LaneQueue queue("Priority", 6);
    const Vehicle normalFirst(201, 0, VehicleType::NORMAL);
    const Vehicle emergencyFirst(202, 1, VehicleType::EMERGENCY);
    const Vehicle normalSecond(203, 2, VehicleType::NORMAL);
    const Vehicle emergencySecond(204, 3, VehicleType::EMERGENCY);

    check(queue.enqueue(normalFirst), "first normal vehicle accepted");
    check(queue.enqueue(emergencyFirst), "first emergency vehicle accepted");
    check(queue.enqueue(normalSecond), "second normal vehicle accepted");
    check(queue.enqueue(emergencySecond), "second emergency vehicle accepted");

    check(queue.dequeue().getId() == emergencyFirst.getId(),
          "first emergency vehicle moves ahead of normal traffic");
    check(queue.dequeue().getId() == emergencySecond.getId(),
          "emergency vehicles retain FIFO order among themselves");
    check(queue.dequeue().getId() == normalFirst.getId(),
          "normal vehicles retain FIFO order after emergency traffic");
    check(queue.dequeue().getId() == normalSecond.getId(),
          "later normal vehicle remains behind earlier normal vehicle");
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
void testMissingConfigurationFileRejected() {
    const char* path = "test_missing_configuration_does_not_exist.tmp";
    std::remove(path);

    SimulationEngine engine;
    bool clearError = false;
    try {
        engine.loadConfigFromFile(path);
    } catch (const std::runtime_error& error) {
        const std::string message = error.what();
        clearError = message.find("Could not open configuration file: ") == 0;
    }
    check(clearError,
          "missing configuration file is rejected with a descriptive error");
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
        checkThrows<std::runtime_error>([&] { engine.loadConfigFromFile(path); }, message);
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

void testEngineDequeuesVehiclesOnlyOnGreen() {
    SimulationEngine engine;
    Intersection intersection("Green-Only-Dequeue");
    intersection.addLane("North", 5);
    intersection.addLane("South", 5);
    intersection.getLanes()[0].enqueue(
        Vehicle(601, 0, VehicleType::NORMAL)
    );
    intersection.getLanes()[1].enqueue(
        Vehicle(602, 0, VehicleType::NORMAL)
    );

    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);

    engine.tick(true);
    check(engine.getStats().totalThroughput() == 1,
          "only the initially green lane is dequeued on the first tick");

    for (int tick = 0; tick < 21; ++tick) {
        engine.tick(true);
    }
    check(engine.getStats().totalThroughput() == 1,
          "red and yellow lane remains queued until it receives green");

    engine.tick(true);
    check(engine.getStats().totalThroughput() == 2,
          "second lane is dequeued when its signal becomes green");
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
void testConnectedVehicleTransferTakesOneTick() {
    SimulationEngine engine;
    Intersection source("One-Tick-Source");
    source.addLane("North", 2);
    Intersection destination("One-Tick-Destination");
    destination.addLane("North", 2);

    source.getLanes()[0].enqueue(
        Vehicle(701, 0, VehicleType::NORMAL)
    );

    engine.addIntersection(source);
    engine.addIntersection(destination);
    engine.loadConfig(0.0, 0.0, false);
    engine.connectIntersections("One-Tick-Source", "One-Tick-Destination");

    engine.tick(true);
    check(engine.getTotalTransfers() == 0,
          "transfer is queued after the source vehicle leaves in tick one");
    check(engine.getStats().totalThroughput() == 0,
          "connected source vehicle is not counted as processed before delivery");

    engine.tick(true);
    check(engine.getTotalTransfers() == 1,
          "pending vehicle is delivered at the start of the following tick");
    check(engine.getStats().totalThroughput() == 1,
          "destination processes the transferred vehicle on the following tick");
}

void testPendingTransferWaitsForDestinationCapacity() {
    SimulationEngine engine;
    Intersection source("Transfer-Source");
    source.addLane("North", 2);
    Intersection destination("Transfer-Destination");
    destination.addLane("Blocker", 2); // Holds the initial green phase.
    destination.addLane("North", 1);   // Matching lane starts red and is full.

    source.getLanes()[0].enqueue(
        Vehicle(901, 0, VehicleType::NORMAL)
    );
    destination.getLanes()[1].enqueue(
        Vehicle(900, 0, VehicleType::NORMAL)
    );

    engine.addIntersection(source);
    engine.addIntersection(destination);
    engine.loadConfig(0.0, 0.0, false);
    engine.connectIntersections("Transfer-Source", "Transfer-Destination");

    // The destination lane is full, so the source vehicle is held upstream
    // (spillback) and transferred only once the destination lane has room.
    for (int tick = 0; tick < 25; ++tick) {
        engine.tick(true);
    }

    check(engine.getTotalTransfers() == 1,
          "pending transfer is delivered after destination capacity becomes available");
    check(engine.getStats().totalThroughput() >= 2,
          "destination processes both its original vehicle and the transferred vehicle");
}

void testConsoleLongLaneAndHighQueueDisplay() {
    const std::string longLaneName = "Northbound_Express_Lane_With_Extended_Name";
    Intersection intersection("Console-Long-Lane");
    intersection.addLane(longLaneName, 30);
    for (int id = 1; id <= 25; ++id) {
        check(intersection.getLanes()[0].enqueue(
                  Vehicle(static_cast<std::uint64_t>(id), 0, VehicleType::NORMAL)),
              "high-queue console fixture accepts vehicles");
    }

    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());
    ConsoleView view;
    view.renderTick(12, intersection.getId(), intersection.getLanes(),
                    intersection.getSignalController(), false);
    std::cout.rdbuf(previous);

    const std::string output = captured.str();
    check(output.find(longLaneName) != std::string::npos,
          "console preserves the complete long lane name");
    check(output.find("25") != std::string::npos,
          "console displays a high queue length");
    check(output.find(std::string(25, '#')) != std::string::npos,
          "console renders the complete high-queue visual bar");
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
void testRepeatedSimulationLifecycle() {
    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());

    for (int iteration = 0; iteration < 20; ++iteration) {
        SimulationEngine engine;
        Intersection intersection("Repeated-Lifecycle-" + std::to_string(iteration));
        intersection.addLane("North", 10);
        intersection.addLane("South", 10);
        engine.addIntersection(intersection);
        engine.loadConfig(0.2, 0.05, false);
        engine.run(100, true);
        check(engine.getCurrentTick() == 100,
              "repeated simulation instance completes its configured run");
    }

    std::cout.rdbuf(previous);
    check(captured.str().find("TMS SIMULATION REPORT") != std::string::npos,
          "repeated simulation lifecycle produces run summaries");
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


int reportedTickCount(const std::string& output) {
    const std::string marker = "TMS SIMULATION REPORT (";
    const std::size_t position = output.find(marker);
    check(position != std::string::npos, "run report is present");
    return std::stoi(output.substr(position + marker.size()));
}

void checkReportedTicksMatchCompletedTicks(const std::string& output,
                                           const SimulationEngine& engine) {
    check(reportedTickCount(output) == engine.getCurrentTick(),
          "report tick count matches the number of completed ticks");
}

void testZeroTickConfigurationMeansOpenEndedRun() {
    const char* path = "test_open_ended_config.tmp";
    {
        std::ofstream file(path);
        file << "total_ticks=0\n"
             << "adaptive_mode=false\n"
             << "queue_capacity=5\n";
    }
    SimulationEngine engine;
    const int configuredTicks = engine.loadConfigFromFile(path);
    check(configuredTicks == 0,
          "total_ticks=0 is accepted as an open-ended run");
    std::remove(path);
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
    checkReportedTicksMatchCompletedTicks(output, engine);
}

void testSigintStopsOpenEndedRun() {
    installSimulationSignalHandlers();
    SimulationEngine engine;
    Intersection intersection("sigint-stop");
    intersection.addLane("North", 15);
    intersection.addLane("South", 15);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.05, false);

    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());
    std::thread signalSender([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::raise(SIGINT);
    });

    engine.run(0, true);
    signalSender.join();
    std::cout.rdbuf(previous);

    const std::string output = captured.str();
    check(output.find("user requested stop") != std::string::npos,
          "SIGINT is handled as a user-requested stop");
    checkReportedTicksMatchCompletedTicks(output, engine);
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

void testAdaptiveGreenExtensionBounds() {
    // Configured base Green = 2 ticks; adaptive cap = base + maxExtension = 6.
    SignalController signals(2, 1, 1);
    signals.addLane("North");
    signals.addLane("South");
    signals.setAdaptiveMode(true, 4);

    const std::map<std::string, std::size_t> heavyQueue = {
        {"North", 50}, {"South", 0}
    };

    for (int tick = 1; tick <= 5; ++tick) {
        signals.updateAdaptiveTimers(heavyQueue);
        signals.updateSignalState(1);
        check(signals.getCurrentPhase("North") == SignalPhase::GREEN,
              "adaptive green remains active before the configured maximum extension expires");
        check(signals.getGreenDuration() <= 6,
              "adaptive green never exceeds configured base plus maximum extension");
    }
    signals.updateAdaptiveTimers(heavyQueue);
    check(signals.getGreenDuration() == 6,
          "extreme queue length is capped at configured base plus maximum extension");
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("North") == SignalPhase::YELLOW,
          "adaptive green transitions at the capped duration");

    signals.updateSignalState(1); // North YELLOW -> RED
    signals.updateSignalState(1); // South GREEN

    const std::map<std::string, std::size_t> lightQueue = {
        {"North", 0}, {"South", 0}
    };
    signals.updateAdaptiveTimers(lightQueue);
    check(signals.getGreenDuration() == 2,
          "low queue restores the configured base green duration, not a hard-coded value");
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("South") == SignalPhase::GREEN,
          "low queue green remains active before the base duration");
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("South") == SignalPhase::YELLOW,
          "low queue green ends at the configured base duration");

    signals.setAdaptiveMode(false);
    check(signals.getGreenDuration() == signals.getBaseGreenDuration(),
          "disabling adaptive mode restores the configured base green duration");

    checkThrows<std::invalid_argument>(
        [] { SignalController s; s.setAdaptiveMode(true, -1); },
        "negative adaptive maximum extension rejected");
}

void testAdaptiveModeCanBeChangedDuringSignalOperation() {
    SignalController signals(2, 1, 1);
    signals.addLane("North");
    signals.addLane("South");

    check(!signals.isAdaptiveMode(),
          "adaptive mode starts disabled");
    signals.updateAdaptiveTimers({{"North", 8}});
    signals.updateSignalState(2);
    check(signals.getCurrentPhase("North") == SignalPhase::YELLOW,
          "disabled adaptive mode does not extend the active green phase");

    signals.updateSignalState(1); // North YELLOW -> RED
    signals.updateSignalState(1); // South GREEN
    signals.setAdaptiveMode(true, 3);
    check(signals.isAdaptiveMode(),
          "adaptive mode can be enabled while the controller is running");
    signals.updateAdaptiveTimers({{"South", 8}});
    signals.updateSignalState(2);
    check(signals.getCurrentPhase("South") == SignalPhase::GREEN,
          "runtime adaptive mode extends the active lane green phase");
    signals.updateSignalState(2);
    check(signals.getCurrentPhase("South") == SignalPhase::YELLOW,
          "runtime adaptive extension ends at its configured duration");
}

void testAdaptiveModeLoadedFromConfiguration() {
    const char* path = "test_adaptive_mode_config.tmp";
    {
        std::ofstream file(path);
        file << "total_ticks=5\n"
             << "arrival_rate=0\n"
             << "emergency_probability=0\n"
             << "adaptive_mode=true\n"
             << "queue_capacity=4\n"
             << "intersection.AdaptiveJunction.lanes=North,South\n";
    }

    SimulationEngine engine;
    check(engine.loadConfigFromFile(path) == 5,
          "adaptive-mode config returns configured run length");
    check(engine.getIntersectionCount() == 1,
          "adaptive-mode config creates its configured intersection");
    check(engine.isAdaptiveMode("AdaptiveJunction"),
          "adaptive_mode=true is applied to the configured intersection");
    checkThrows<std::out_of_range>(
        [&] { engine.isAdaptiveMode("MissingJunction"); },
        "adaptive-mode query rejects an unknown intersection");
    std::remove(path);
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


void testSiblingLaneQueueIsolation() {
    LaneQueue north("North", 2);
    LaneQueue south("South", 2);
    const Vehicle northVehicle(901, 0, VehicleType::NORMAL);
    const Vehicle southVehicle(902, 0, VehicleType::NORMAL);

    check(north.enqueue(northVehicle), "North accepts its vehicle");
    check(north.size() == 1 && south.size() == 0,
          "enqueue in North does not change South queue state");
    check(south.enqueue(southVehicle), "South accepts its vehicle");
    check(north.size() == 1 && south.size() == 1,
          "sibling lanes retain independent queue contents");
    check(north.dequeue().getId() == northVehicle.getId(),
          "North dequeue returns only its own vehicle");
    check(south.size() == 1 && south.dequeue().getId() == southVehicle.getId(),
          "North operations do not corrupt South queue");
}

void testMaxQueueLengthOneBoundary() {
    StatisticsCollector stats;
    stats.updateMaxQueueLength("BoundaryJunction", "North", 1);
    stats.updateMaxQueueLength("BoundaryJunction", "North", 0);

    std::ostringstream report;
    std::streambuf* previous = std::cout.rdbuf(report.rdbuf());
    stats.printEndOfRunReport(1);
    std::cout.rdbuf(previous);

    const std::string output = report.str();
    const std::size_t laneRow = output.find("BoundaryJunction");
    check(laneRow != std::string::npos,
          "single-entry queue appears in the statistics breakdown");
    check(output.find("1", laneRow) != std::string::npos,
          "maximum queue length remains one after a later zero observation");
}

void testZeroThroughputIntersectionAppearsInAggregateReport() {
    StatisticsCollector stats;
    stats.updateMaxQueueLength("EmptyJunction", "North", 0);

    std::ostringstream report;
    std::streambuf* previous = std::cout.rdbuf(report.rdbuf());
    stats.printEndOfRunReport(0);
    std::cout.rdbuf(previous);

    const std::string output = report.str();
    check(stats.totalThroughput() == 0,
          "an intersection with no dequeues contributes zero aggregate throughput");
    check(output.find("EmptyJunction") != std::string::npos,
          "zero-throughput intersection is retained in the lane breakdown");
    check(output.find("EmptyJunction") != std::string::npos &&
          output.find("North") != std::string::npos,
          "zero-throughput lane is identified in the report");
}

void testSimultaneousOverflowAcrossEveryLane() {
    Intersection intersection("All-Lanes-Full");
    const std::vector<std::string> laneNames = {"North", "South", "East", "West"};
    for (const std::string& laneName : laneNames) {
        intersection.addLane(laneName, 1);
    }

    for (std::size_t i = 0; i < laneNames.size(); ++i) {
        LaneQueue& lane = intersection.getLanes()[i];
        check(lane.enqueue(Vehicle(2000 + i, 0, VehicleType::NORMAL)),
              "each lane accepts its first vehicle up to capacity");
    }

    for (std::size_t i = 0; i < laneNames.size(); ++i) {
        LaneQueue& lane = intersection.getLanes()[i];
        check(!lane.enqueue(Vehicle(3000 + i, 1, VehicleType::NORMAL)),
              "overflow is rejected independently on every full lane");
        check(lane.size() == 1 && lane.isFull(),
              "simultaneous overflow leaves every lane at its capacity");
    }
}

void testZeroEmergencyProbabilityProducesNormalVehicles() {
    VehicleGenerator generator(1.0, 0.0, 123);
    for (int tick = 0; tick < 100; ++tick) {
        const Vehicle vehicle =
            generator.generateVehicle("Central", "North", tick);
        check(!vehicle.isEmergency(),
              "zero emergency probability generates only normal vehicles");
    }
}


void testEmergencyProbabilityOneAlwaysGeneratesEmergencyVehicles() {
    VehicleGenerator generator(1.0, 1.0, 456);
    for (int tick = 0; tick < 100; ++tick) {
        const Vehicle vehicle =
            generator.generateVehicle("Central", "North", tick);
        check(vehicle.isEmergency(),
              "emergency probability one generates only emergency vehicles");
    }
}


// ---- Sprint 3: bounded emergency preemption (TMS-F-032) ----
int countGreenLanes(const SignalController& signals, const std::vector<std::string>& lanes) {
    int greens = 0;
    for (const std::string& lane : lanes)
        if (signals.getCurrentPhase(lane) == SignalPhase::GREEN) ++greens;
    return greens;
}

void testBoundedEmergencyPreemptionDelay() {
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    // Request preemption at every possible point of the first full cycle and
    // confirm the emergency lane is Green within yellow + all-red ticks.
    for (int offset = 0; offset < 23; ++offset) {
        SignalController signals(10, 3, 10);
        for (const std::string& lane : lanes) signals.addLane(lane);
        for (int tick = 0; tick < offset; ++tick) signals.updateSignalState(1);

        const bool northWasGreen = signals.getCurrentPhase("North") == SignalPhase::GREEN;
        check(signals.preempt("East"), "emergency request for a red lane is accepted");

        int updates = 0;
        bool sawYellowBeforeGrant = !northWasGreen;
        while (signals.getCurrentPhase("East") != SignalPhase::GREEN && updates < 50) {
            signals.updateSignalState(1);
            ++updates;
            if (signals.getCurrentPhase("North") == SignalPhase::YELLOW) sawYellowBeforeGrant = true;
            check(countGreenLanes(signals, lanes) <= 1,
                  "preemption never creates two simultaneous green lanes");
        }
        // The engine raises the request and applies the first signal update in
        // the same tick, so the delay in ticks is one less than the update count.
        const int delay = updates - 1;
        check(delay <= signals.worstCasePreemptionDelay(),
              "emergency lane receives green within yellow + all-red delay bound");
        check(delay <= signals.getMaxPreemptionDelay(),
              "emergency lane receives green within the configured maximum preemption delay");
        check(sawYellowBeforeGrant,
              "interrupted green lane passes through yellow before the emergency grant");
        check(!signals.isPreemptionPending(), "preemption request is cleared once granted");
    }
}

void testPreemptionDelayConfigurationBounds() {
    SignalController signals(10, 3, 10);
    check(signals.worstCasePreemptionDelay() == 4, "worst-case delay is yellow plus one all-red tick");
    check(signals.getMaxPreemptionDelay() == 4, "default preemption bound equals the worst-case delay");
    checkThrows<std::invalid_argument>([&] { signals.setMaxPreemptionDelay(3); },
                                       "preemption bound shorter than yellow clearance is rejected");
    signals.setMaxPreemptionDelay(6);
    check(signals.getMaxPreemptionDelay() == 6, "valid preemption bound is retained");
    signals.setDurations(15, 1, 5);
    check(signals.getGreenDuration() == 15 && signals.getYellowDuration() == 1 &&
          signals.getRedDuration() == 5, "signal durations can be reconfigured");
    check(signals.worstCasePreemptionDelay() == 2, "worst-case delay follows the new yellow duration");
    checkThrows<std::invalid_argument>([&] { signals.setDurations(10, 0, 10); },
                                       "zero yellow duration is rejected");
}

void testEnginePreemptsWithinConfiguredDelay() {
    SimulationEngine engine;
    Intersection intersection("Preempt-Junction");
    intersection.addLane("North", 10);
    intersection.addLane("East", 10);
    // Pre-load the emergency vehicle so the request is raised on tick 3 while North is Green.
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    for (int tick = 0; tick < 2; ++tick) engine.tick(true);

    const SignalController& signals = engine.getIntersections().front().getSignalController();
    check(signals.getCurrentPhase("North") == SignalPhase::GREEN &&
          signals.getCurrentPhase("East") == SignalPhase::RED,
          "north is green and east is red before the emergency request");

    SimulationEngine emergencyEngine;
    Intersection seeded("Preempt-Junction");
    seeded.addLane("North", 10);
    seeded.addLane("East", 10);
    seeded.getLanes()[1].enqueue(Vehicle(9001, 0, VehicleType::EMERGENCY));
    emergencyEngine.addIntersection(seeded);
    emergencyEngine.loadConfig(0.0, 0.0, false);

    const SignalController& live = emergencyEngine.getIntersections().front().getSignalController();
    int grantedTick = -1;
    for (int tick = 0; tick < 20 && grantedTick < 0; ++tick) {
        emergencyEngine.tick(true);
        if (live.getCurrentPhase("East") == SignalPhase::GREEN) grantedTick = emergencyEngine.getCurrentTick();
    }
    // The request is raised on tick 1, so the delay is grantedTick - 1.
    check(grantedTick >= 0, "engine grants green to the emergency lane");
    check(grantedTick - 1 <= live.getMaxPreemptionDelay(),
          "engine-level preemption delay is within the configured bound");
    check(emergencyEngine.getStats().totalPreemptions() == 1, "engine logs exactly one preemption event");
    check(emergencyEngine.getStats().totalThroughput() == 1,
          "emergency vehicle is dequeued once its lane is green");
}

// ---- Sprint 3: signal timings via configuration (TMS-F-020 / TMS-F-060) ----
void writeTimingConfig(const char* path, const std::string& extra) {
    std::ofstream file(path);
    file << "total_ticks=10\narrival_rate=0.5\nemergency_probability=0\n"
         << "adaptive_mode=false\nqueue_capacity=5\n"
         << "intersection.Timed.lanes=North,South\n" << extra;
}

void testSignalTimingsLoadedFromConfiguration() {
    const char* path = "test_signal_timing_config.tmp";
    writeTimingConfig(path, "green_duration=15\nyellow_duration=2\nred_duration=5\nmax_preemption_delay=6\n"
                            "max_green_extension=7\n");
    {
        SimulationEngine engine;
        check(engine.loadConfigFromFile(path) == 10, "timing config returns run length");
        const SignalController& signals = engine.getIntersections().front().getSignalController();
        check(signals.getMaxGreenExtension() == 7, "max_green_extension is applied from configuration");
        check(signals.getGreenDuration() == 15, "green_duration is applied from configuration");
        check(signals.getYellowDuration() == 2, "yellow_duration is applied from configuration");
        check(signals.getRedDuration() == 5, "red_duration is applied from configuration");
        check(signals.getMaxPreemptionDelay() == 6, "max_preemption_delay is applied from configuration");
    }
    writeTimingConfig(path, "");
    {
        SimulationEngine engine;
        engine.loadConfigFromFile(path);
        const SignalController& signals = engine.getIntersections().front().getSignalController();
        check(signals.getGreenDuration() == 10 && signals.getYellowDuration() == 3 &&
              signals.getRedDuration() == 10 && signals.getMaxPreemptionDelay() == 4,
              "omitted timing keys keep documented defaults");
    }
    const char* invalid[] = {"green_duration=0\n", "yellow_duration=-1\n", "red_duration=abc\n",
                             "green_duration=1.5\n", "yellow_duration=2\nmax_preemption_delay=2\n",
                             "red_duration=10001\n", "max_green_extension=-1\n"};
    for (const char* extra : invalid) {
        writeTimingConfig(path, extra);
        SimulationEngine engine;
        checkThrows<std::runtime_error>([&] { engine.loadConfigFromFile(path); },
                                        "invalid signal timing configuration is rejected");
    }
    std::remove(path);
}

// ---- Sprint 3: transfers are bounded by downstream capacity (PRJ-SR-004 / TMS-NF-001) ----
void testPendingTransfersBoundedByDownstreamCapacity() {
    SimulationEngine engine;
    const std::size_t capacity = 15;
    for (int i = 1; i <= 8; ++i) {
        Intersection intersection("Chain-" + std::to_string(i));
        for (const char* lane : {"North", "South", "East", "West"}) intersection.addLane(lane, capacity);
        engine.addIntersection(intersection);
    }
    for (int i = 1; i < 8; ++i)
        engine.connectIntersections("Chain-" + std::to_string(i), "Chain-" + std::to_string(i + 1));
    engine.loadConfig(0.95, 0.10, true);

    // Each of the 7 linked destinations has 4 lanes of capacity 15.
    const std::size_t bound = 7 * 4 * capacity;
    std::size_t peakPending = 0;
    bool lanesWithinCapacity = true;
    for (int tick = 0; tick < 3000; ++tick) {
        engine.tick(true);
        peakPending = std::max(peakPending, engine.getPendingTransferCount());
        for (const auto& intersection : engine.getIntersections())
            for (const auto& lane : intersection.getLanes())
                lanesWithinCapacity = lanesWithinCapacity && lane.size() <= lane.capacity();
    }
    check(peakPending <= bound, "in-flight transfers never exceed total downstream lane capacity");
    check(lanesWithinCapacity, "no lane ever exceeds its configured capacity under chained overload");
    check(engine.getTotalTransfers() > 0 && engine.getStats().totalThroughput() > 0,
          "chained overload still transfers and processes vehicles");
}

// ---- Sprint 3: console columns stay aligned with long lane names (TMS-NF-004) ----
void testConsoleColumnsStayAlignedWithLongLaneNames() {
    Intersection intersection("Alignment");
    intersection.addLane("NorthBoundExpressLaneExtended", 20);
    intersection.addLane("South", 20);
    intersection.addLane("E", 20);
    for (int id = 1; id <= 15; ++id)
        intersection.getLanes()[0].enqueue(Vehicle(id, 0, VehicleType::NORMAL));

    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());
    ConsoleView().renderTick(7, intersection.getId(), intersection.getLanes(),
                             intersection.getSignalController(), false);
    std::cout.rdbuf(previous);

    std::istringstream lines(captured.str());
    std::string line;
    std::vector<std::size_t> firstBar, secondBar;
    while (std::getline(lines, line)) {
        const std::size_t first = line.find(" | ");
        if (first == std::string::npos || line.find("[Intersection") != std::string::npos) continue;
        firstBar.push_back(first);
        secondBar.push_back(line.find(" | ", first + 3));
    }
    check(firstBar.size() == 4, "header and three lane rows are rendered");
    bool aligned = true;
    for (std::size_t i = 1; i < firstBar.size(); ++i)
        aligned = aligned && firstBar[i] == firstBar[0] && secondBar[i] == secondBar[0];
    check(aligned, "signal and queue columns stay aligned when one lane name is very long");
    check(captured.str().find(std::string(15, '#')) != std::string::npos,
          "high queue bar is rendered in full");
}

// ---- Sprint 3: per-intersection totals (TMS-F-042 / TMS-F-052) ----
void testPerIntersectionTotalsSumToAggregate() {
    SimulationEngine engine;
    Intersection busy("Busy");
    busy.addLane("North", 10);
    busy.addLane("South", 10);
    Intersection quiet("Quiet");
    quiet.addLane("North", 10);
    engine.addIntersection(busy);
    engine.addIntersection(quiet);
    engine.loadConfig(0.6, 0.0, false);
    engine.configureLaneArrivalRate("South", 0.2);

    std::ostringstream captured;
    std::streambuf* previous = std::cout.rdbuf(captured.rdbuf());
    engine.run(200, true);
    std::cout.rdbuf(previous);

    const auto totals = engine.getStats().intersectionTotals();
    check(totals.size() == 2, "both intersections appear in per-intersection totals");
    std::int64_t sum = 0;
    for (const auto& entry : totals) sum += entry.second.throughput;
    check(sum == engine.getStats().totalThroughput(),
          "per-intersection throughput sums exactly to the system aggregate");
    check(engine.getStats().intersectionThroughput("Busy") > 0 &&
          engine.getStats().intersectionThroughput("Quiet") > 0,
          "each intersection reports its own throughput");
    check(engine.getStats().intersectionThroughput("Missing") == 0,
          "unknown intersection reports zero throughput");
    const std::string report = captured.str();
    check(report.find("Per-Intersection Totals:") != std::string::npos,
          "end-of-run report contains per-intersection totals");
    check(report.find("SYSTEM TOTAL") != std::string::npos,
          "end-of-run report contains the system-wide total row");
}

}
// Runs one test function and prints its name with the number of checks it passed.
#define RUN_TEST(name) do { const int before = passed; name(); \
    std::cout << "[PASS] " #name " (" << (passed - before) << " checks)" << std::endl; } while (0)

int main() {
    RUN_TEST(testProbabilityBoundaries);
    RUN_TEST(testVehicleIdsAreUniqueAcrossLanesAndIntersections);
    RUN_TEST(testArrivalRates);
    RUN_TEST(testConfigurationUpdates);
    RUN_TEST(testInvalidConfiguration);
    RUN_TEST(testEmergencyQueuePriorityAndStableOrdering);
    RUN_TEST(testQueueCapacityAndEmptyDequeue);
    RUN_TEST(testMalformedConfiguration);
    RUN_TEST(testMissingConfigurationFileRejected);
    RUN_TEST(testConfigurationValidationBoundaries);
    RUN_TEST(testEngineDequeuesVehiclesOnlyOnGreen);
    RUN_TEST(testConfigDrivenIntersections);
    RUN_TEST(testConnectedVehicleTransferTakesOneTick);
    RUN_TEST(testPendingTransferWaitsForDestinationCapacity);
    RUN_TEST(testConsoleLongLaneAndHighQueueDisplay);
    RUN_TEST(testConsoleStatusFields);
    RUN_TEST(testRepeatedSimulationLifecycle);
    RUN_TEST(testTenThousandTickStability);
    RUN_TEST(testZeroTickConfigurationMeansOpenEndedRun);
    RUN_TEST(testManualStopProducesPartialSummary);
    RUN_TEST(testSigintStopsOpenEndedRun);
    RUN_TEST(testSignalPhaseTransitions);
    RUN_TEST(testNoSimultaneousGreenSignals);
    RUN_TEST(testAdaptiveGreenExtensionBounds);
    RUN_TEST(testAdaptiveModeCanBeChangedDuringSignalOperation);
    RUN_TEST(testAdaptiveModeLoadedFromConfiguration);
    RUN_TEST(testEmergencyPreemption);
    RUN_TEST(testStatisticsCalculations);
    RUN_TEST(testMaxQueueLengthOneBoundary);
    RUN_TEST(testZeroThroughputIntersectionAppearsInAggregateReport);
    RUN_TEST(testSimultaneousOverflowAcrossEveryLane);
    RUN_TEST(testIntersectionValidationAndSignalRegistration);
    RUN_TEST(testSiblingLaneQueueIsolation);
    RUN_TEST(testZeroEmergencyProbabilityProducesNormalVehicles);
    RUN_TEST(testEmergencyProbabilityOneAlwaysGeneratesEmergencyVehicles);
    RUN_TEST(testBoundedEmergencyPreemptionDelay);
    RUN_TEST(testPreemptionDelayConfigurationBounds);
    RUN_TEST(testEnginePreemptsWithinConfiguredDelay);
    RUN_TEST(testSignalTimingsLoadedFromConfiguration);
    RUN_TEST(testPerIntersectionTotalsSumToAggregate);
    RUN_TEST(testConsoleColumnsStayAlignedWithLongLaneNames);
    RUN_TEST(testPendingTransfersBoundedByDownstreamCapacity);
    std::cout << "All " << passed << " checks passed." << std::endl;
    return EXIT_SUCCESS;
}
