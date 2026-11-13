#include "../include/SimulationEngine.h"
#include "../include/LaneQueue.h"
#include "../include/ConsoleView.h"
#include "../vehicle/VehicleGenerator.h"
#include <chrono>
#include <csignal>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
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

    // The source vehicle is scheduled while the destination lane is full.
    // The transfer must remain pending until the destination lane can accept it.
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
    SignalController signals(2, 1, 1);
    signals.addLane("North");
    signals.addLane("South");
    signals.setAdaptiveMode(true, 4);

    const std::map<std::string, std::size_t> heavyQueue = {
        {"North", 6}, {"South", 0}
    };

    // The active lane's green duration must extend, but never beyond
    // the configured base duration (10) plus maxExtension (4).
    for (int tick = 1; tick <= 13; ++tick) {
        signals.updateAdaptiveTimers(heavyQueue);
        signals.updateSignalState(1);
        check(signals.getCurrentPhase("North") == SignalPhase::GREEN,
              "adaptive green remains active before the configured maximum extension expires");
    }
    signals.updateAdaptiveTimers(heavyQueue);
    signals.updateSignalState(1);
    check(signals.getCurrentPhase("North") == SignalPhase::YELLOW,
          "adaptive green transitions at the capped duration");

    // A low queue restores the base green duration for the next cycle.
    signals.updateSignalState(1); // North YELLOW -> RED
    signals.updateSignalState(1); // South GREEN

    // Let the South phase complete at its still-extended duration before
    // checking the next North cycle.
    for (int tick = 0; tick < 14; ++tick) {
        signals.updateSignalState(1);
    }
    check(signals.getCurrentPhase("South") == SignalPhase::YELLOW,
          "second lane completes its extended green phase");
    signals.updateSignalState(1); // South YELLOW -> RED
    signals.updateSignalState(1); // North GREEN

    const std::map<std::string, std::size_t> lightQueue = {
        {"North", 0}, {"South", 0}
    };
    signals.updateAdaptiveTimers(lightQueue);
    for (int tick = 1; tick <= 10; ++tick) {
        signals.updateSignalState(1);
        if (tick < 10) {
            check(signals.getCurrentPhase("North") == SignalPhase::GREEN,
                  "low queue uses base green duration");
        }
    }
    check(signals.getCurrentPhase("North") == SignalPhase::YELLOW,
          "low queue green ends at base duration");

    checkThrows<std::invalid_argument>(
        [] { SignalController s; s.setAdaptiveMode(true, -1); },
        "negative adaptive maximum extension rejected");
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

void testZeroEmergencyProbabilityProducesNormalVehicles() {
    VehicleGenerator generator(1.0, 0.0, 123);
    for (int tick = 0; tick < 100; ++tick) {
        const Vehicle vehicle =
            generator.generateVehicle("Central", "North", tick);
        check(!vehicle.isEmergency(),
              "zero emergency probability generates only normal vehicles");
    }
}

}
int main() {
    testProbabilityBoundaries();
    testVehicleIdsAreUniqueAcrossLanesAndIntersections();
    testArrivalRates();
    testConfigurationUpdates();
    testInvalidConfiguration();
    testEmergencyQueuePriorityAndStableOrdering();
    testQueueCapacityAndEmptyDequeue();
    testMalformedConfiguration();
    testConfigurationValidationBoundaries();
    testEngineDequeuesVehiclesOnlyOnGreen();
    testConfigDrivenIntersections();
    testConnectedVehicleTransferTakesOneTick();
    testPendingTransferWaitsForDestinationCapacity();
    testConsoleLongLaneAndHighQueueDisplay();
    testConsoleStatusFields();
    testRepeatedSimulationLifecycle();
    testTenThousandTickStability();
    testZeroTickConfigurationMeansOpenEndedRun();
    testManualStopProducesPartialSummary();
    testSigintStopsOpenEndedRun();
    testSignalPhaseTransitions();
    testNoSimultaneousGreenSignals();
    testAdaptiveGreenExtensionBounds();
    testEmergencyPreemption();
    testStatisticsCalculations();
    testIntersectionValidationAndSignalRegistration();
    testSiblingLaneQueueIsolation();
    testZeroEmergencyProbabilityProducesNormalVehicles();
    std::cout << "All " << passed << " checks passed." << std::endl;
    return EXIT_SUCCESS;
}
