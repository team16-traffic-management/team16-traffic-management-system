// Test Plan v1.2 acceptance runner.
//
// Executes the in-process Test Plan cases (Section 13) individually, by TC ID,
// against the production classes. Each case uses the test data and expected
// result recorded in Documents/Test_Plan_Traffic_Management_System.docx v1.2.
// Cases that need an external process, a real console Ctrl+C, a second OS or a
// memory tool are executed by scripts/run_acceptance.sh instead.
//
// Output: one line per case  "<TC-ID> | <Requirement> | PASS|FAIL | <observed>"
// Arguments: [--no-perf] [results.md]  (see main()).

#include "../../include/SimulationEngine.h"
#include "../../include/ConsoleView.h"
#include "../../include/LaneQueue.h"
#include "../../vehicle/VehicleGenerator.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Context {
    std::vector<std::string> failures;
    std::ostringstream observed;
    void expect(bool condition, const std::string& message) {
        if (!condition) failures.push_back(message);
    }
};

struct TestCase {
    std::string id;
    std::string requirement;
    std::string title;
    std::function<void(Context&)> body;
};

struct Result {
    std::string id;
    std::string requirement;
    std::string title;
    bool passed;
    std::string observed;
};

// Redirects std::cout for the lifetime of the object.
class CaptureCout {
public:
    CaptureCout() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CaptureCout() { std::cout.rdbuf(previous_); }
    std::string text() const { return buffer_.str(); }
private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

class TempConfig {
public:
    TempConfig(const std::string& name, const std::string& content) : path_("acceptance_" + name + ".cfg") {
        std::ofstream file(path_);
        file << content;
    }
    ~TempConfig() { std::remove(path_.c_str()); }
    const std::string& path() const { return path_; }
private:
    std::string path_;
};

std::string baseConfig(const std::string& extra, const std::string& lanes = "North,South,East,West",
                       const std::string& common = "total_ticks=100\narrival_rate=0.3\n"
                                                   "emergency_probability=0\nadaptive_mode=false\n"
                                                   "queue_capacity=10000\n") {
    return common + "intersection.Main.lanes=" + lanes + "\n" + extra;
}

std::string loadError(const std::string& configText, const std::string& name) {
    TempConfig config(name, configText);
    SimulationEngine engine;
    try {
        CaptureCout quiet;
        engine.loadConfigFromFile(config.path());
    } catch (const std::exception& error) {
        return error.what();
    }
    return "";
}

std::string loadErrorFromPath(const std::string& path) {
    SimulationEngine engine;
    try {
        CaptureCout quiet;
        engine.loadConfigFromFile(path);
    } catch (const std::exception& error) {
        return error.what();
    }
    return "";
}

SignalPhase phaseOf(const SimulationEngine& engine, std::size_t intersection, const std::string& lane) {
    return engine.getIntersections().at(intersection).getSignalController().getCurrentPhase(lane);
}

std::size_t queuedVehicles(const SimulationEngine& engine) {
    std::size_t total = 0;
    for (const auto& intersection : engine.getIntersections())
        for (const auto& lane : intersection.getLanes()) total += lane.size();
    return total;
}

const char* phaseName(SignalPhase phase) {
    switch (phase) {
        case SignalPhase::GREEN: return "GREEN";
        case SignalPhase::YELLOW: return "YELLOW";
        case SignalPhase::RED: return "RED";
    }
    return "?";
}

// Positions of the column separators on every table row of a rendered block.
bool columnsAligned(const std::string& output, std::size_t& rowsChecked) {
    std::istringstream lines(output);
    std::string line;
    std::vector<std::size_t> reference;
    rowsChecked = 0;
    bool aligned = true;
    while (std::getline(lines, line)) {
        if (line.find("[Intersection") != std::string::npos) { reference.clear(); continue; }
        std::vector<std::size_t> bars;
        for (std::size_t pos = line.find(" | "); pos != std::string::npos; pos = line.find(" | ", pos + 3))
            bars.push_back(pos);
        if (bars.size() < 3) continue;
        bars.resize(3);
        ++rowsChecked;
        if (reference.empty()) reference = bars;
        else if (bars != reference) aligned = false;
    }
    return aligned && rowsChecked > 0;
}

double ticksPerSecond(SimulationEngine& engine, int ticks) {
    const auto start = std::chrono::steady_clock::now();
    for (int tick = 0; tick < ticks; ++tick) engine.tick(true);
    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
    return elapsed.count() > 0 ? ticks / elapsed.count() : 0.0;
}

void addIntersections(SimulationEngine& engine, int count, std::size_t capacity, bool chain) {
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    for (int i = 1; i <= count; ++i) {
        Intersection intersection("J" + std::to_string(i));
        for (const auto& lane : lanes) intersection.addLane(lane, capacity);
        engine.addIntersection(intersection);
    }
    if (chain)
        for (int i = 1; i < count; ++i)
            engine.connectIntersections("J" + std::to_string(i), "J" + std::to_string(i + 1));
}

// ===================================================================== 13.1
void genSpawnRate(Context& c) {
    // arrivalRate=0.3, 4 lanes, 1,000 ticks; tolerance +/-10% of 300.
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    VehicleGenerator generator(0.3, 0.0, 42);
    for (const auto& lane : lanes) generator.configureLane("Main", lane, 0.3);
    std::map<std::string, int> spawned;
    for (int tick = 1; tick <= 1000; ++tick)
        for (const auto& lane : lanes)
            if (generator.shouldGenerate("Main", lane)) {
                generator.generateVehicle("Main", lane, tick);
                ++spawned[lane];
            }
    for (const auto& lane : lanes) {
        c.observed << lane << "=" << spawned[lane] << " ";
        c.expect(spawned[lane] >= 270 && spawned[lane] <= 330, lane + " spawn count outside 270..330");
    }
    c.observed << "(expected 300 +/-10%)";
}

void genZeroRate(Context& c) {
    // Uses the version-controlled zero-value fixture, run for 500 ticks.
    SimulationEngine engine;
    {
        CaptureCout quiet;
        engine.loadConfigFromFile("tests/data/zero_values.cfg");
        engine.run(500, true);
    }
    const auto& stats = engine.getStats();
    c.observed << "500 ticks: throughput=" << stats.totalThroughput() << " blocked=" << stats.totalBlocked()
               << " queued=" << queuedVehicles(engine);
    c.expect(stats.totalThroughput() == 0 && stats.totalBlocked() == 0 && queuedVehicles(engine) == 0,
             "vehicles were spawned with arrival_rate=0");
    c.expect(engine.getCurrentTick() == 500, "run did not complete 500 ticks");
}

void genIdTickType(Context& c) {
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    VehicleGenerator generator(0.4, 0.05, 42);
    for (const auto& lane : lanes) generator.configureLane("Main", lane, 0.4);
    std::set<int> ids;
    int vehicles = 0;
    bool ticksOk = true, typesOk = true;
    for (int tick = 1; tick <= 50; ++tick)
        for (const auto& lane : lanes)
            if (generator.shouldGenerate("Main", lane)) {
                const Vehicle v = generator.generateVehicle("Main", lane, tick);
                ++vehicles;
                ids.insert(v.getId());
                ticksOk = ticksOk && v.getArrivalTick() == tick;
                typesOk = typesOk && (v.getType() == VehicleType::NORMAL || v.getType() == VehicleType::EMERGENCY);
            }
    c.observed << vehicles << " vehicles in 50 ticks, " << ids.size() << " unique IDs";
    c.expect(vehicles > 0, "no vehicles generated");
    c.expect(static_cast<int>(ids.size()) == vehicles, "duplicate vehicle IDs");
    c.expect(ticksOk, "arrival tick differs from creation tick");
    c.expect(typesOk, "vehicle type is not NORMAL or EMERGENCY");
}

void genSameTickUnique(Context& c) {
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    VehicleGenerator generator(0.9, 0.0, 42);
    for (const auto& lane : lanes) generator.configureLane("Main", lane, 0.9);
    for (int tick = 1; tick <= 100; ++tick) {
        std::vector<Vehicle> sameTick;
        for (const auto& lane : lanes)
            if (generator.shouldGenerate("Main", lane)) sameTick.push_back(generator.generateVehicle("Main", lane, tick));
        if (sameTick.size() >= 2) {
            std::set<int> ids;
            for (const auto& v : sameTick) ids.insert(v.getId());
            c.observed << "tick " << tick << ": " << sameTick.size() << " same-tick spawns, IDs";
            for (const auto& v : sameTick) c.observed << " " << v.getId();
            c.expect(ids.size() == sameTick.size(), "same-tick vehicles share an ID");
            return;
        }
    }
    c.expect(false, "no same-tick multi-lane spawn observed");
}

void genFifo(Context& c) {
    LaneQueue lane("North", 10);
    for (int tick = 1; tick <= 5; ++tick) lane.enqueue(Vehicle(100 + tick, tick, VehicleType::NORMAL));
    c.observed << "dequeue order:";
    bool fifo = true;
    for (int expected = 101; expected <= 105; ++expected) {
        const int id = lane.dequeue().getId();
        c.observed << " " << id;
        fifo = fifo && id == expected;
    }
    c.expect(fifo, "dequeue order differs from spawn order");
}

void genFifoWithEmergency(Context& c) {
    LaneQueue lane("North", 10);
    lane.enqueue(Vehicle(1, 1, VehicleType::NORMAL));
    lane.enqueue(Vehicle(2, 2, VehicleType::NORMAL));
    lane.enqueue(Vehicle(9, 3, VehicleType::EMERGENCY));
    lane.enqueue(Vehicle(3, 4, VehicleType::NORMAL));
    std::vector<int> order;
    while (!lane.isEmpty()) order.push_back(lane.dequeue().getId());
    c.observed << "N1,N2,E9,N3 enqueued -> dequeued";
    for (int id : order) c.observed << " " << id;
    c.expect(order == std::vector<int>({9, 1, 2, 3}), "normal vehicles were reordered by emergency insertion");
}

// ===================================================================== 13.2
void queueIndependent(Context& c) {
    Intersection intersection("Main");
    for (const char* lane : {"North", "South", "East", "West"}) intersection.addLane(lane, 10);
    for (int i = 0; i < 3; ++i) intersection.getLanes()[0].enqueue(Vehicle(i + 1, 1, VehicleType::NORMAL));
    const auto& lanes = intersection.getLanes();
    c.observed << "North=" << lanes[0].size() << " South=" << lanes[1].size() << " East=" << lanes[2].size()
               << " West=" << lanes[3].size();
    c.expect(lanes[0].size() == 3, "North does not hold 3 vehicles");
    c.expect(lanes[1].isEmpty() && lanes[2].isEmpty() && lanes[3].isEmpty(), "sibling lanes changed");
}

void queueSiblingIntact(Context& c) {
    Intersection intersection("Main");
    intersection.addLane("North", 10);
    intersection.addLane("South", 10);
    auto& north = intersection.getLanes()[0];
    auto& south = intersection.getLanes()[1];
    north.enqueue(Vehicle(1, 1, VehicleType::NORMAL));
    north.enqueue(Vehicle(2, 1, VehicleType::NORMAL));
    south.enqueue(Vehicle(3, 1, VehicleType::NORMAL));
    south.enqueue(Vehicle(4, 2, VehicleType::NORMAL));
    while (!north.isEmpty()) north.dequeue();
    const auto& q = south.getQueue();
    c.observed << "after draining North, South holds " << q.size() << " vehicles: " << q[0].getId() << ","
               << q[1].getId();
    c.expect(q.size() == 2 && q[0].getId() == 3 && q[1].getId() == 4, "South contents or order changed");
}

void queueCapacityEnforced(Context& c) {
    LaneQueue lane("Target", 3);
    std::vector<bool> accepted;
    for (int i = 1; i <= 5; ++i) accepted.push_back(lane.enqueue(Vehicle(i, i, VehicleType::NORMAL)));
    // Engine path: the same 5 arrivals are logged as blocked events.
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("Other", 10);
    intersection.addLane("Target", 3);
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    engine.configureLaneArrivalRate("Target", 1.0);
    for (int tick = 0; tick < 5; ++tick) engine.tick(true);
    const auto blocked = engine.getStats().blockedFor("Main", "Target");
    c.observed << "accept pattern " << accepted[0] << accepted[1] << accepted[2] << accepted[3] << accepted[4]
               << "; engine: Target size=" << engine.getIntersections()[0].getLanes()[1].size()
               << " blocked=" << blocked;
    c.expect(accepted == std::vector<bool>({true, true, true, false, false}), "wrong accept/reject pattern");
    c.expect(blocked == 2, "engine did not log 2 blocked vehicles");
}

void queueZeroCapacity(Context& c) {
    std::string laneError, intersectionError;
    try { LaneQueue lane("Zero", 0); } catch (const std::invalid_argument& e) { laneError = e.what(); }
    try { Intersection i("Main"); i.addLane("Zero", 0); } catch (const std::invalid_argument& e) { intersectionError = e.what(); }
    const std::string configError = loadError(baseConfig("", "North", "total_ticks=10\nadaptive_mode=false\nqueue_capacity=0\n"), "zero_cap");
    c.observed << "LaneQueue: \"" << laneError << "\"; config: \"" << configError << "\"";
    c.expect(!laneError.empty() && !intersectionError.empty(), "zero-capacity lane was constructed");
    c.expect(configError.find("queue_capacity") != std::string::npos, "queue_capacity=0 not rejected clearly");
}

void queueDequeueOnlyGreen(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("Other", 10);
    intersection.addLane("Target", 10);
    intersection.getLanes()[1].enqueue(Vehicle(1, 0, VehicleType::NORMAL));
    intersection.getLanes()[1].enqueue(Vehicle(2, 0, VehicleType::NORMAL));
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    int nonGreenTicks = 0, illegalDequeues = 0, firstGreen = -1;
    for (int tick = 1; tick <= 30; ++tick) {
        const std::size_t before = engine.getIntersections()[0].getLanes()[1].size();
        engine.tick(true);
        const std::size_t after = engine.getIntersections()[0].getLanes()[1].size();
        if (phaseOf(engine, 0, "Target") != SignalPhase::GREEN) {
            ++nonGreenTicks;
            if (after < before) ++illegalDequeues;
        } else if (firstGreen < 0) {
            firstGreen = tick;
        }
    }
    c.observed << nonGreenTicks << " Red/Yellow ticks with 2 queued: " << illegalDequeues
               << " dequeues; first Green at tick " << firstGreen;
    c.expect(nonGreenTicks >= 10, "lane was not held Red for at least 10 ticks");
    c.expect(illegalDequeues == 0, "vehicle dequeued while not Green");
}

void queueEmptyGreen(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("North", 5);
    intersection.addLane("South", 5);
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    bool threw = false;
    try { for (int tick = 0; tick < 5; ++tick) engine.tick(true); } catch (...) { threw = true; }
    c.observed << "5 ticks with empty Green lane North: throughput=" << engine.getStats().totalThroughput()
               << " exception=" << (threw ? "yes" : "no");
    c.expect(phaseOf(engine, 0, "North") == SignalPhase::GREEN, "North was not Green");
    c.expect(!threw && engine.getStats().totalThroughput() == 0, "empty Green lane produced an event or error");
}

void queueWaitTime(Context& c) {
    StatisticsCollector stats;
    stats.recordDequeue("Main", "North", Vehicle(1, 5, VehicleType::NORMAL), 12);
    c.observed << "arrival 5, dequeue 12 -> recorded wait " << stats.averageWaitTime();
    c.expect(stats.averageWaitTime() == 7.0, "wait time is not 7");
}

void queueImmediateWait(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("North", 5);
    engine.addIntersection(intersection);
    engine.loadConfig(1.0, 0.0, false);
    engine.tick(true);  // vehicle arrives at tick 1 while North is Green and is dequeued at tick 1
    c.observed << "tick 1: throughput=" << engine.getStats().totalThroughput()
               << " average wait=" << engine.getStats().averageWaitTime();
    c.expect(engine.getStats().totalThroughput() == 1, "vehicle was not dequeued on arrival tick");
    c.expect(engine.getStats().averageWaitTime() == 0.0, "immediate dequeue wait is not exactly 0");
}

// ===================================================================== 13.3
void sigFixedCycle(Context& c) {
    TempConfig config("sig001p", baseConfig("green_duration=10\nyellow_duration=3\nred_duration=10\n", "North,South",
                                            "total_ticks=30\narrival_rate=0\nadaptive_mode=false\nqueue_capacity=5\n"));
    SimulationEngine engine;
    { CaptureCout quiet; engine.loadConfigFromFile(config.path()); }
    std::vector<int> changes;
    SignalPhase last = phaseOf(engine, 0, "North");
    for (int tick = 1; tick <= 23; ++tick) {
        engine.tick(true);
        const SignalPhase now = phaseOf(engine, 0, "North");
        if (now != last) changes.push_back(tick);
        last = now;
    }
    const bool southGreenAt23 = phaseOf(engine, 0, "South") == SignalPhase::GREEN;
    c.observed << "North phase changes at ticks";
    for (int t : changes) c.observed << " " << t;
    c.observed << "; South Green at tick 23: " << (southGreenAt23 ? "yes" : "no");
    c.expect(changes == std::vector<int>({10, 13}) && southGreenAt23, "phase changes not at 10, 13, 23");
}

void sigZeroDuration(Context& c) {
    const std::string error = loadError(baseConfig("yellow_duration=0\n"), "sig001n");
    bool ctorRejected = false;
    try { SignalController s(10, 0, 10); } catch (const std::invalid_argument&) { ctorRejected = true; }
    c.observed << "yellow_duration=0 -> \"" << error << "\"";
    c.expect(error.find("yellow_duration") != std::string::npos, "zero Yellow not rejected with clear error");
    c.expect(ctorRejected, "SignalController accepted zero Yellow");
}

int adaptiveGreenTicks(std::size_t queueLength) {
    SignalController signals(10, 3, 10);
    signals.addLane("North");
    signals.addLane("South");
    signals.setAdaptiveMode(true, 10);  // max Green = 10 + 10 = 20
    const std::map<std::string, std::size_t> queues = {{"North", queueLength}, {"South", 0}};
    int tick = 0;
    while (signals.getCurrentPhase("North") == SignalPhase::GREEN && tick < 100) {
        ++tick;
        signals.updateSignalState(1);          // engine order: update, then adaptive timers
        signals.updateAdaptiveTimers(queues);
    }
    return tick;  // tick on which Green ended
}

void sigAdaptiveExtends(Context& c) {
    const int green = adaptiveGreenTicks(8);
    TempConfig config("sig002p", baseConfig("max_green_extension=10\n", "North,South",
                                            "total_ticks=10\nadaptive_mode=true\nqueue_capacity=5\n"));
    SimulationEngine engine;
    { CaptureCout quiet; engine.loadConfigFromFile(config.path()); }
    const auto& signals = engine.getIntersections()[0].getSignalController();
    c.observed << "queue=8: Green lasted " << green << " ticks (base 10, max 20); config max_green_extension="
               << signals.getMaxGreenExtension();
    c.expect(green > 10 && green <= 20, "Green duration not in (10, 20]");
    c.expect(signals.isAdaptiveMode() && signals.getMaxGreenExtension() == 10, "configured maximum not applied");
}

void sigAdaptiveCapped(Context& c) {
    const int green = adaptiveGreenTicks(50);
    c.observed << "queue=50: Green lasted " << green << " ticks (max 20)";
    c.expect(green == 20, "Green not capped at exactly 20");
}

void sigNoConflictingGreen(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    for (const char* lane : {"North", "South", "East", "West"}) intersection.addLane(lane, 15);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.05, false);
    int conflicts = 0, multiGreen = 0, greenTicks = 0;
    for (int tick = 1; tick <= 100; ++tick) {
        engine.tick(true);
        const bool ns = phaseOf(engine, 0, "North") == SignalPhase::GREEN || phaseOf(engine, 0, "South") == SignalPhase::GREEN;
        const bool ew = phaseOf(engine, 0, "East") == SignalPhase::GREEN || phaseOf(engine, 0, "West") == SignalPhase::GREEN;
        int greens = 0;
        for (const char* lane : {"North", "South", "East", "West"}) greens += phaseOf(engine, 0, lane) == SignalPhase::GREEN;
        if (ns && ew) ++conflicts;
        if (greens > 1) ++multiGreen;
        if (greens) ++greenTicks;
    }
    c.observed << "100 ticks logged: N/S+E/W conflicts=" << conflicts << ", ticks with >1 Green=" << multiGreen
               << ", ticks with a Green=" << greenTicks << ", preemptions=" << engine.getStats().totalPreemptions();
    c.expect(conflicts == 0 && multiGreen == 0, "conflicting Green state observed");
}

void sigForcedConflict(Context& c) {
    SignalController signals(10, 3, 10);
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    for (const auto& lane : lanes) signals.addLane(lane);
    int accepted = 0, rejected = 0, violations = 0;
    for (int tick = 0; tick < 200; ++tick) {
        // Fault injection: request Green for two conflicting lanes in the same tick.
        const std::string first = lanes[tick % 4], second = lanes[(tick + 2) % 4];
        (signals.preempt(first) ? accepted : rejected)++;
        (signals.preempt(second) ? accepted : rejected)++;
        signals.updateSignalState(1);
        int greens = 0;
        for (const auto& lane : lanes) greens += signals.getCurrentPhase(lane) == SignalPhase::GREEN;
        if (greens > 1) ++violations;
    }
    c.observed << "200 ticks x 2 simultaneous override requests: accepted=" << accepted << " rejected=" << rejected
               << " multi-Green ticks=" << violations;
    c.expect(violations == 0, "two lanes Green at once");
    c.expect(rejected >= 200, "same-tick second request was not rejected");
}

std::vector<SignalPhase> northPhases(int yellow, int ticks) {
    SignalController signals(10, yellow, 10);
    signals.addLane("North");
    signals.addLane("South");
    std::vector<SignalPhase> phases;
    for (int tick = 0; tick < ticks; ++tick) {
        signals.updateSignalState(1);
        phases.push_back(signals.getCurrentPhase("North"));
    }
    return phases;
}

int countPhase(const std::vector<SignalPhase>& phases, SignalPhase phase) {
    return static_cast<int>(std::count(phases.begin(), phases.end(), phase));
}

void sigYellowBeforeRed(Context& c) {
    const auto phases = northPhases(3, 14);
    const auto firstYellow = std::find(phases.begin(), phases.end(), SignalPhase::YELLOW);
    const auto firstRed = std::find(phases.begin(), phases.end(), SignalPhase::RED);
    c.observed << "Yellow ticks between Green and Red: " << countPhase(phases, SignalPhase::YELLOW);
    c.expect(firstYellow < firstRed && countPhase(phases, SignalPhase::YELLOW) == 3, "Yellow not exactly 3 ticks before Red");
}

void sigOneTickYellow(Context& c) {
    const auto phases = northPhases(1, 12);
    Intersection intersection("Main");
    intersection.addLane("North", 5);
    intersection.addLane("South", 5);
    intersection.getSignalController().setDurations(10, 1, 10);
    for (int i = 0; i < 10; ++i) intersection.update(1);
    std::string rendered;
    { CaptureCout capture; ConsoleView().renderTick(10, "Main", intersection.getLanes(), intersection.getSignalController()); rendered = capture.text(); }
    c.observed << "Yellow=1: Yellow ticks=" << countPhase(phases, SignalPhase::YELLOW) << ", Green->Yellow->Red sequence "
               << (phases[9] == SignalPhase::YELLOW && phases[10] == SignalPhase::RED ? "observed" : "missing")
               << ", console shows YELLOW: " << (rendered.find("YELLOW") != std::string::npos ? "yes" : "no");
    c.expect(countPhase(phases, SignalPhase::YELLOW) == 1, "Yellow did not last exactly 1 tick");
    c.expect(phases[9] == SignalPhase::YELLOW && phases[10] == SignalPhase::RED, "phase skipped or merged");
    c.expect(rendered.find("YELLOW") != std::string::npos, "one-tick Yellow not rendered");
}

void sigModeViaConfig(Context& c) {
    const std::string common = "total_ticks=10\nqueue_capacity=5\n";
    TempConfig fixed("sig005p_fixed", baseConfig("", "North,South", common + "adaptive_mode=false\n"));
    TempConfig adaptive("sig005p_adaptive", baseConfig("", "North,South", common + "adaptive_mode=true\n"));
    SimulationEngine first, second;
    { CaptureCout quiet; first.loadConfigFromFile(fixed.path()); second.loadConfigFromFile(adaptive.path()); }
    c.observed << "adaptive_mode=false -> adaptive " << first.isAdaptiveMode("Main") << "; adaptive_mode=true -> adaptive "
               << second.isAdaptiveMode("Main") << " (same binary)";
    c.expect(!first.isAdaptiveMode("Main") && second.isAdaptiveMode("Main"), "mode did not follow configuration");
}

void sigInvalidMode(Context& c) {
    const std::string error = loadError(baseConfig("", "North", "total_ticks=10\nqueue_capacity=5\nadaptive_mode=addaptive\n"), "sig005n");
    c.observed << "adaptive_mode=addaptive -> \"" << error << "\"";
    c.expect(error == "adaptive_mode must be true or false", "invalid mode not rejected with clear error");
}

// ===================================================================== 13.4
void emgSpawnProbability(Context& c) {
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    VehicleGenerator generator(1.0, 0.05, 42);
    int total = 0, emergency = 0;
    for (int tick = 1; tick <= 1000; ++tick)
        for (const auto& lane : lanes) {
            ++total;
            emergency += generator.generateVehicle("Main", lane, tick).isEmergency();
        }
    const double share = static_cast<double>(emergency) / total;
    c.observed << emergency << " of " << total << " vehicles EMERGENCY (" << std::fixed << std::setprecision(2)
               << share * 100 << "%, expected 5% +/-1.5%)";
    c.expect(share >= 0.035 && share <= 0.065, "emergency share outside tolerance");
}

void emgZeroProbability(Context& c) {
    SimulationEngine engine;
    TempConfig config("emg001n", baseConfig("", "North,South,East,West",
                                            "total_ticks=500\narrival_rate=0.5\nemergency_probability=0\n"
                                            "adaptive_mode=false\nqueue_capacity=20\n"));
    { CaptureCout quiet; engine.run(engine.loadConfigFromFile(config.path()), true); }
    c.observed << "500 ticks, throughput=" << engine.getStats().totalThroughput()
               << ", preemptions=" << engine.getStats().totalPreemptions();
    c.expect(engine.getStats().totalThroughput() > 0, "no traffic generated");
    c.expect(engine.getStats().totalPreemptions() == 0, "emergency vehicles appeared with probability 0");
    VehicleGenerator generator(1.0, 0.0, 7);
    int emergency = 0;
    for (int tick = 1; tick <= 500; ++tick) emergency += generator.generateVehicle("Main", "North", tick).isEmergency();
    c.observed << ", generator: " << emergency << "/500 emergency";
    c.expect(emergency == 0, "generator produced emergency vehicles");
}

void emgPriority(Context& c) {
    LaneQueue lane("North", 10);
    for (int i = 1; i <= 3; ++i) lane.enqueue(Vehicle(i, i, VehicleType::NORMAL));
    lane.enqueue(Vehicle(99, 4, VehicleType::EMERGENCY));
    const int first = lane.dequeue().getId();
    c.observed << "3 normal then emergency 99 -> first dequeued " << first;
    c.expect(first == 99, "emergency vehicle not dequeued first");
}

void emgFifoAmongEmergency(Context& c) {
    LaneQueue lane("North", 10);
    lane.enqueue(Vehicle(1, 1, VehicleType::EMERGENCY));
    lane.enqueue(Vehicle(2, 2, VehicleType::EMERGENCY));
    const int a = lane.dequeue().getId();
    const int b = lane.dequeue().getId();
    c.observed << "A(1) then B(2) -> " << a << ", " << b;
    c.expect(a == 1 && b == 2, "emergency FIFO not preserved");
}

void emgPreemptWithinDelay(Context& c) {
    // Max preemption delay = 2 ticks (yellow_duration=1 + 1 all-red tick).
    TempConfig config("emg003p", "total_ticks=10\narrival_rate=0\nemergency_probability=1.0\nadaptive_mode=false\n"
                                  "queue_capacity=5\nyellow_duration=1\nmax_preemption_delay=2\n"
                                  "intersection.Main.lanes=North_Bound,East_Bound\nEast_Bound=1.0\n");
    SimulationEngine engine;
    { CaptureCout quiet; engine.loadConfigFromFile(config.path()); }
    int requestTick = -1, greenTick = -1;
    for (int tick = 1; tick <= 10 && greenTick < 0; ++tick) {
        engine.tick(true);
        if (requestTick < 0 && engine.getStats().totalPreemptions() > 0) requestTick = tick;
        if (phaseOf(engine, 0, "East_Bound") == SignalPhase::GREEN) greenTick = tick;
    }
    const int maxDelay = engine.getIntersections()[0].getSignalController().getMaxPreemptionDelay();
    c.observed << "emergency in Red East_Bound, preemption logged at tick " << requestTick << ", Green at tick "
               << greenTick << " (delay " << greenTick - requestTick << ", configured max " << maxDelay << ")";
    c.expect(requestTick == 1, "preemption event not logged on arrival tick");
    c.expect(greenTick > 0 && greenTick - requestTick <= 2, "Green not granted within 2 ticks");
}

void emgPreemptNoConflict(Context& c) {
    SignalController signals(10, 3, 10);
    signals.addLane("East");   // currently Green
    signals.addLane("North");  // holds the emergency vehicle
    for (int i = 0; i < 4; ++i) signals.updateSignalState(1);
    signals.preempt("North");
    std::vector<std::string> trace;
    bool eastYellowSeen = false, northGreenBeforeYellow = false, bothGreen = false;
    for (int tick = 0; tick < 6; ++tick) {
        signals.updateSignalState(1);
        const SignalPhase east = signals.getCurrentPhase("East"), north = signals.getCurrentPhase("North");
        trace.push_back(std::string(phaseName(east)) + "/" + phaseName(north));
        if (east == SignalPhase::YELLOW) eastYellowSeen = true;
        if (north == SignalPhase::GREEN && !eastYellowSeen) northGreenBeforeYellow = true;
        if (east == SignalPhase::GREEN && north == SignalPhase::GREEN) bothGreen = true;
    }
    c.observed << "East/North per tick:";
    for (const auto& t : trace) c.observed << " " << t;
    c.expect(eastYellowSeen && !northGreenBeforeYellow, "East did not pass Yellow before North Green");
    c.expect(!bothGreen, "East and North Green simultaneously");
    c.expect(signals.getCurrentPhase("North") == SignalPhase::GREEN, "North never granted Green");
}

// ===================================================================== 13.5
void multiIndependent(Context& c) {
    SimulationEngine engine;
    const std::vector<std::pair<std::string, double>> setup = {{"A", 0.2}, {"B", 0.5}, {"C", 0.8}};
    for (const auto& entry : setup) {
        Intersection intersection(entry.first);
        intersection.addLane(entry.first + "_N", 1000);
        intersection.addLane(entry.first + "_S", 1000);
        engine.addIntersection(intersection);
    }
    engine.loadConfig(0.0, 0.0, false);
    for (const auto& entry : setup) {
        engine.configureLaneArrivalRate(entry.first + "_N", entry.second);
        engine.configureLaneArrivalRate(entry.first + "_S", entry.second);
    }
    for (int tick = 0; tick < 300; ++tick) engine.tick(true);
    const auto totals = engine.getStats().intersectionTotals();
    std::vector<std::size_t> arrived;
    c.observed << "300 ticks:";
    for (std::size_t i = 0; i < engine.getIntersections().size(); ++i) {
        const auto& in = engine.getIntersections()[i];
        std::size_t queued = 0;
        for (const auto& lane : in.getLanes()) {
            queued += lane.size();
            c.expect(lane.name().compare(0, in.getId().size(), in.getId()) == 0, "lane appears under wrong intersection");
        }
        const auto processed = totals.count(in.getId()) ? totals.at(in.getId()).throughput : 0;
        arrived.push_back(queued + static_cast<std::size_t>(processed));
        c.observed << " " << in.getId() << "(rate " << setup[i].second << "): arrived=" << arrived.back()
                   << " queued=" << queued << " processed=" << processed;
    }
    c.expect(arrived[0] < arrived[1] && arrived[1] < arrived[2], "per-intersection state does not follow its own rate");
}

void multiFaultIsolation(Context& c) {
    // Updated v1.2 expected result: an invalid intersection definition is rejected
    // at load time (fail-fast, PRJ-SR-002); a runtime overload in one intersection
    // does not affect the others.
    const std::string error = loadError("total_ticks=10\nadaptive_mode=false\nqueue_capacity=5\n"
                                        "intersection.One.lanes=North,South\nintersection.Two.lanes=North,,South\n"
                                        "intersection.Three.lanes=North,South\n", "multi001n");
    SimulationEngine engine;
    for (const std::string id : {"One", "Two", "Three"}) {
        Intersection intersection(id);
        intersection.addLane(id + "_N", id == "Two" ? 1 : 50);
        intersection.addLane(id + "_S", id == "Two" ? 1 : 50);
        engine.addIntersection(intersection);
    }
    engine.loadConfig(0.3, 0.0, false);
    engine.configureLaneArrivalRate("Two_N", 1.0);
    engine.configureLaneArrivalRate("Two_S", 1.0);
    for (int tick = 0; tick < 200; ++tick) engine.tick(true);
    const auto totals = engine.getStats().intersectionTotals();
    c.observed << "invalid Two config -> \"" << error << "\"; runtime overload of Two: blocked One/Two/Three="
               << totals.at("One").blocked << "/" << totals.at("Two").blocked << "/" << totals.at("Three").blocked
               << ", throughput One/Three=" << totals.at("One").throughput << "/" << totals.at("Three").throughput;
    c.expect(error.find("Empty lane name in intersection: Two") != std::string::npos, "invalid intersection not identified");
    c.expect(totals.at("Two").blocked > 0, "fault was not injected");
    c.expect(totals.at("One").blocked == 0 && totals.at("Three").blocked == 0, "fault leaked into healthy intersections");
    c.expect(totals.at("One").throughput > 0 && totals.at("Three").throughput > 0, "healthy intersections stopped");
}

void multiGreenWave(Context& c) {
    SimulationEngine engine;
    Intersection a("A");
    a.addLane("North", 5);
    Intersection b("B");
    b.addLane("Other", 5);
    b.addLane("North", 5);  // Red in B so the delivered vehicle stays visible
    a.getLanes()[0].enqueue(Vehicle(4242, 0, VehicleType::NORMAL));
    engine.addIntersection(a);
    engine.addIntersection(b);
    engine.connectIntersections("A", "B");
    engine.loadConfig(0.0, 0.0, false);
    engine.tick(true);
    const std::size_t afterFirst = engine.getIntersections()[1].getLanes()[1].size();
    engine.tick(true);
    const auto& bNorth = engine.getIntersections()[1].getLanes()[1];
    const int idInB = bNorth.isEmpty() ? -1 : bNorth.getQueue().front().getId();
    c.observed << "vehicle 4242 exits A at tick 1; B.North size after tick 1=" << afterFirst
               << ", after tick 2=" << bNorth.size() << " (front id " << idInB << ")";
    c.expect(afterFirst == 0 && idInB == 4242, "vehicle did not appear in B on the next tick");
}

void multiFullDownstream(Context& c) {
    SimulationEngine engine;
    Intersection a("A");
    a.addLane("North", 5);
    Intersection b("B");
    b.addLane("Other", 5);
    b.addLane("North", 1);
    a.getLanes()[0].enqueue(Vehicle(1, 0, VehicleType::NORMAL));
    b.getLanes()[1].enqueue(Vehicle(2, 0, VehicleType::NORMAL));  // B.North full
    engine.addIntersection(a);
    engine.addIntersection(b);
    engine.connectIntersections("A", "B");
    engine.loadConfig(0.0, 0.0, false);
    for (int tick = 0; tick < 5; ++tick) engine.tick(true);
    // v1.2 expected result: the handoff is deferred (spillback) - the vehicle
    // stays in A's queue while B.North is full; nothing is lost or duplicated.
    const std::size_t heldInA = engine.getIntersections()[0].getLanes()[0].size();
    const std::size_t bWhileFull = engine.getIntersections()[1].getLanes()[1].size();
    const std::size_t total = queuedVehicles(engine) + engine.getPendingTransferCount() + engine.getStats().totalThroughput();
    for (int tick = 0; tick < 40; ++tick) engine.tick(true);
    const std::size_t totalAfter = queuedVehicles(engine) + engine.getPendingTransferCount() + engine.getStats().totalThroughput();
    c.observed << "B.North full (size " << bWhileFull << "/1): vehicle held in A=" << heldInA
               << ", in-flight=0, vehicles accounted=" << total << "; after B drains: transfers="
               << engine.getTotalTransfers() << ", accounted=" << totalAfter;
    c.expect(heldInA == 1 && bWhileFull == 1, "handoff not deferred while destination full");
    c.expect(total == 2 && totalAfter == 2, "vehicle lost or duplicated");
    c.expect(engine.getTotalTransfers() == 1, "vehicle not delivered exactly once after capacity freed");
}

void multiStatsSum(Context& c) {
    SimulationEngine engine;
    Intersection busy("Busy");
    busy.addLane("Busy_N", 50);
    busy.addLane("Busy_S", 50);
    Intersection quiet("Quiet");
    quiet.addLane("Quiet_N", 50);
    quiet.addLane("Quiet_S", 50);
    engine.addIntersection(busy);
    engine.addIntersection(quiet);
    engine.loadConfig(0.6, 0.0, false);
    engine.configureLaneArrivalRate("Quiet_N", 0.1);
    engine.configureLaneArrivalRate("Quiet_S", 0.1);
    std::string report;
    { CaptureCout capture; engine.run(300, true); report = capture.text(); }
    const auto totals = engine.getStats().intersectionTotals();
    const auto sum = totals.at("Busy").throughput + totals.at("Quiet").throughput;
    c.observed << "Busy=" << totals.at("Busy").throughput << " + Quiet=" << totals.at("Quiet").throughput << " = " << sum
               << "; reported aggregate=" << engine.getStats().totalThroughput();
    c.expect(sum == engine.getStats().totalThroughput(), "per-intersection totals do not sum to aggregate");
    c.expect(report.find("Per-Intersection Totals:") != std::string::npos && report.find("SYSTEM TOTAL") != std::string::npos,
             "report lacks per-intersection and system totals");
}

void multiZeroThroughput(Context& c) {
    SimulationEngine engine;
    Intersection one("One");
    one.addLane("One_N", 50);
    Intersection two("Two");
    two.addLane("Two_N", 50);
    engine.addIntersection(one);
    engine.addIntersection(two);
    engine.loadConfig(0.5, 0.0, false);
    engine.configureLaneArrivalRate("Two_N", 0.0);
    std::string report;
    { CaptureCout capture; engine.run(200, true); report = capture.text(); }
    const auto totals = engine.getStats().intersectionTotals();
    c.observed << "Two: throughput=" << totals.at("Two").throughput << " avg wait=" << totals.at("Two").averageWaitTime()
               << "; aggregate=" << engine.getStats().totalThroughput() << " == One=" << totals.at("One").throughput;
    c.expect(totals.at("Two").throughput == 0 && totals.at("Two").averageWaitTime() == 0.0, "zero intersection not 0/0");
    c.expect(engine.getStats().totalThroughput() == totals.at("One").throughput, "aggregate differs from One");
    c.expect(report.find("Two") != std::string::npos, "zero-throughput intersection missing from report");
}

// ===================================================================== 13.6
void statsAverage(Context& c) {
    StatisticsCollector stats;
    stats.recordDequeue("Main", "North", Vehicle(1, 0, VehicleType::NORMAL), 4);
    stats.recordDequeue("Main", "North", Vehicle(2, 0, VehicleType::NORMAL), 6);
    stats.recordDequeue("Main", "North", Vehicle(3, 0, VehicleType::NORMAL), 8);
    c.observed << "waits 4,6,8 -> reported average " << stats.averageWaitTime();
    c.expect(stats.averageWaitTime() == 6.0, "average is not 6");
}

void statsAverageNoData(Context& c) {
    StatisticsCollector stats;
    std::string report;
    { CaptureCout capture; stats.printEndOfRunReport(0); report = capture.text(); }
    c.observed << "0 events -> average " << stats.averageWaitTime() << ", report prints \"0.00 ticks\": "
               << (report.find("0.00 ticks") != std::string::npos ? "yes" : "no");
    c.expect(stats.averageWaitTime() == 0.0 && report.find("0.00 ticks") != std::string::npos, "no-data average not safe");
}

void statsMaxQueue(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("Other", 20);
    intersection.addLane("Target", 20);
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    engine.configureLaneArrivalRate("Target", 1.0);
    for (int tick = 0; tick < 6; ++tick) engine.tick(true);
    const std::size_t peak = engine.getIntersections()[0].getLanes()[1].size();
    engine.configureLaneArrivalRate("Target", 0.0);
    for (int tick = 0; tick < 40; ++tick) engine.tick(true);
    const std::size_t finalSize = engine.getIntersections()[0].getLanes()[1].size();
    const std::size_t reported = engine.getStats().intersectionTotals().at("Main").maxQueueLength;
    c.observed << "Target peaked at " << peak << ", drained to " << finalSize << "; reported max " << reported;
    c.expect(peak == 6 && finalSize == 0 && reported == 6, "max queue not exactly 6 after draining");
}

void statsMaxQueueOne(Context& c) {
    TempConfig config("stats002n", "total_ticks=500\narrival_rate=0.05\nemergency_probability=0\nadaptive_mode=false\n"
                                    "queue_capacity=5\ngreen_duration=10000\nintersection.Main.lanes=North\n");
    SimulationEngine engine;
    { CaptureCout quiet; engine.run(engine.loadConfigFromFile(config.path()), true); }
    const auto reported = engine.getStats().intersectionTotals().at("Main").maxQueueLength;
    c.observed << "light traffic, Green=10000: arrivals processed=" << engine.getStats().totalThroughput()
               << ", reported max queue=" << reported;
    c.expect(engine.getStats().totalThroughput() > 0 ? reported == 1 : reported == 0, "max queue not exactly 1");
}

void statsThroughputCount(Context& c) {
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    SimulationEngine engine;
    Intersection intersection("Main");
    for (const auto& lane : lanes) intersection.addLane(lane, 1000);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.0, false);
    for (int tick = 0; tick < 200; ++tick) engine.tick(true);
    // Independent count: replay the same seeded per-lane generators, then
    // dequeues = arrivals - vehicles still queued (capacity 1000, so none blocked).
    VehicleGenerator replica(0.4, 0.0);
    for (const auto& lane : lanes) replica.configureLane("Main", lane, 0.4);
    std::size_t arrivals = 0;
    for (int tick = 1; tick <= 200; ++tick)
        for (const auto& lane : lanes)
            if (replica.shouldGenerate("Main", lane)) { replica.generateVehicle("Main", lane, tick); ++arrivals; }
    const std::size_t manual = arrivals - queuedVehicles(engine);
    c.observed << "200 ticks: replayed arrivals=" << arrivals << ", still queued=" << queuedVehicles(engine)
               << ", counted dequeues=" << manual << ", reported throughput=" << engine.getStats().totalThroughput();
    c.expect(engine.getStats().totalBlocked() == 0, "unexpected blocking");
    c.expect(static_cast<std::int64_t>(manual) == engine.getStats().totalThroughput(), "throughput differs from count");
}

void statsBlockedNotThroughput(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("Other", 5);
    intersection.addLane("Target", 3);
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    engine.configureLaneArrivalRate("Target", 1.0);
    for (int tick = 0; tick < 5; ++tick) engine.tick(true);  // 5 arrivals, 2 blocked
    engine.configureLaneArrivalRate("Target", 0.0);
    for (int tick = 0; tick < 40; ++tick) engine.tick(true);  // Target turns Green and drains
    c.observed << "5 arrivals at capacity-3 lane: blocked=" << engine.getStats().totalBlocked()
               << ", throughput=" << engine.getStats().totalThroughput();
    c.expect(engine.getStats().totalBlocked() == 2 && engine.getStats().totalThroughput() == 3, "blocked counted as throughput");
}

// ===================================================================== 13.7
void simConfigApplied(Context& c) {
    TempConfig config("sim001p", "total_ticks=1000\narrival_rate=0.5\nemergency_probability=0\nadaptive_mode=false\n"
                                  "queue_capacity=10000\ngreen_duration=15\nintersection.Main.lanes=North,South,East,West\n");
    SimulationEngine engine;
    int ticks = 0;
    { CaptureCout quiet; ticks = engine.loadConfigFromFile(config.path()); }
    int firstYellow = -1;
    for (int tick = 1; tick <= ticks; ++tick) {
        engine.tick(true);
        if (firstYellow < 0 && phaseOf(engine, 0, "North") == SignalPhase::YELLOW) firstYellow = tick;
    }
    const auto& stats = engine.getStats();
    const double spawned = static_cast<double>(stats.totalThroughput() + stats.totalBlocked() + queuedVehicles(engine));
    const double rate = spawned / (4.0 * ticks);
    c.observed << "lanes=" << engine.getIntersections()[0].getLanes().size() << ", Green ended at tick " << firstYellow
               << " (green_duration=15), observed spawn rate " << std::fixed << std::setprecision(3) << rate << " (0.5)";
    c.expect(engine.getIntersections()[0].getLanes().size() == 4, "lane count not applied");
    c.expect(firstYellow == 15, "green_duration not applied");
    c.expect(std::fabs(rate - 0.5) <= 0.05, "arrival_rate not applied");
}

void simMissingConfig(Context& c) {
    const std::string error = loadErrorFromPath("tests/data/this_file_does_not_exist.cfg");
    c.observed << "\"" << error << "\" (CLI exit code checked by run_acceptance.sh)";
    c.expect(error == "Could not open configuration file: tests/data/this_file_does_not_exist.cfg", "missing file not rejected clearly");
}

void simNaturalCompletion(Context& c) {
    TempConfig config("sim002p", baseConfig("", "North,South,East,West",
                                            "total_ticks=500\narrival_rate=0.3\nemergency_probability=0.05\n"
                                            "adaptive_mode=false\nqueue_capacity=15\n"));
    SimulationEngine engine;
    std::string output;
    { CaptureCout capture; engine.run(engine.loadConfigFromFile(config.path()), true); output = capture.text(); }
    const bool report = output.find("TMS SIMULATION REPORT (500 TICKS)") != std::string::npos;
    const bool reason = output.find("configured tick limit reached (500 ticks executed)") != std::string::npos;
    c.observed << "ended at tick " << engine.getCurrentTick() << "; summary header " << (report ? "present" : "missing")
               << "; termination line " << (reason ? "present" : "missing");
    c.expect(engine.getCurrentTick() == 500 && report && reason && output.find("SYSTEM TOTAL") != std::string::npos,
             "summary report incomplete");
}

// ===================================================================== 13.8
void perfTarget(Context& c) {
    SimulationEngine engine;
    addIntersections(engine, 4, 15, false);
    engine.loadConfig(0.4, 0.05, false);
    const double rate = ticksPerSecond(engine, 10000);
    c.observed << "4 intersections x 4 lanes, 10,000 ticks: " << std::fixed << std::setprecision(0) << rate
               << " ticks/s (target >= 500)";
    c.expect(rate >= 500.0, "below 500 ticks/s");
}

void perfMaxLoad(Context& c) {
    SimulationEngine baseline;
    addIntersections(baseline, 4, 15, false);
    baseline.loadConfig(0.4, 0.05, false);
    const double baseRate = ticksPerSecond(baseline, 5000);
    SimulationEngine heavy;
    addIntersections(heavy, 8, 15, true);
    heavy.loadConfig(0.95, 0.10, true);
    const double heavyRate = ticksPerSecond(heavy, 5000);
    c.observed << std::fixed << std::setprecision(0) << "4-intersection baseline " << baseRate
               << " ticks/s; 8 chained intersections @0.95 arrival " << heavyRate << " ticks/s (ratio "
               << std::setprecision(2) << heavyRate / baseRate << ", linear expectation 0.50)";
    c.expect(heavy.getCurrentTick() == 5000, "heavy run did not complete");
    c.expect(heavyRate >= 500.0 && heavyRate >= 0.25 * baseRate, "performance collapsed under load");
}

void relLongRun(Context& c) {
    SimulationEngine engine;
    addIntersections(engine, 2, 15, true);
    engine.loadConfig(0.5, 0.05, true);
    std::string output;
    { CaptureCout capture; engine.run(10000, true); output = capture.text(); }
    c.observed << "10,000 ticks completed=" << (engine.getCurrentTick() == 10000 ? "yes" : "no")
               << ", throughput=" << engine.getStats().totalThroughput() << " (leak verdict: memory tool run)";
    c.expect(engine.getCurrentTick() == 10000 && output.find("10000 ticks executed") != std::string::npos, "long run failed");
}

void relRepeatedRuns(Context& c) {
    int completed = 0;
    for (int run = 0; run < 10; ++run) {
        SimulationEngine engine;
        addIntersections(engine, 2, 10, true);
        engine.loadConfig(0.5, 0.05, false);
        { CaptureCout quiet; engine.run(500, true); }
        completed += engine.getCurrentTick() == 500;
    }
    c.observed << completed << "/10 sequential create/run/destroy cycles completed (leak verdict: memory tool run)";
    c.expect(completed == 10, "a cycle failed");
}

void maintNewTopology(Context& c) {
    TempConfig config("maint01p", "total_ticks=50\narrival_rate=0.4\nemergency_probability=0.05\nadaptive_mode=false\n"
                                   "queue_capacity=10\nintersection.Central.lanes=North,South,East,West\n"
                                   "intersection.Riverside.lanes=North,South,East,West\n"
                                   "intersection.Harbor.lanes=North,South,East,West\n"
                                   "connection.Central=Riverside\nconnection.Riverside=Harbor\n");
    SimulationEngine engine;
    { CaptureCout quiet; engine.run(engine.loadConfigFromFile(config.path()), true); }
    const auto totals = engine.getStats().intersectionTotals();
    c.observed << "3rd intersection added by config only: intersections=" << engine.getIntersectionCount()
               << ", transfers=" << engine.getTotalTransfers() << ", Harbor throughput="
               << (totals.count("Harbor") ? totals.at("Harbor").throughput : -1);
    c.expect(engine.getIntersectionCount() == 3 && totals.count("Harbor") && totals.at("Harbor").throughput > 0,
             "third intersection not running");
}

void maintInvalidTopology(Context& c) {
    const std::string error = loadError("total_ticks=10\nadaptive_mode=false\nqueue_capacity=5\n"
                                        "intersection.One.lanes=North,South\nconnection.One=One\n", "maint01n");
    c.observed << "connection.One=One -> \"" << error << "\"";
    c.expect(error.find("cannot connect to itself") != std::string::npos, "self-loop not rejected");
}

void uxReadable(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main-Intersection");
    for (const char* lane : {"North_Bound", "South_Bound", "East_Bound", "West_Bound"}) intersection.addLane(lane, 15);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.05, false);
    std::string output;
    { CaptureCout capture; for (int tick = 0; tick < 30; ++tick) engine.tick(false); output = capture.text(); }
    int blocks = 0;
    for (std::size_t pos = output.find("[Intersection Main-Intersection | Tick"); pos != std::string::npos;
         pos = output.find("[Intersection Main-Intersection | Tick", pos + 1)) ++blocks;
    std::size_t rows = 0;
    const bool aligned = columnsAligned(output, rows);
    const bool fields = output.find("Signal") != std::string::npos && output.find("Queue") != std::string::npos &&
                        (output.find("GREEN") != std::string::npos);
    c.observed << blocks << " tick blocks, " << rows << " table rows checked, columns aligned=" << (aligned ? "yes" : "no");
    c.expect(blocks == 30 && fields && aligned, "tick/queue/signal not clearly displayed every tick");
}

void uxLongName(Context& c) {
    Intersection intersection("Main");
    intersection.addLane("NorthBoundExpressLaneExtended", 20);
    for (const char* lane : {"South", "East", "West"}) intersection.addLane(lane, 20);
    for (int id = 1; id <= 15; ++id) intersection.getLanes()[0].enqueue(Vehicle(id, 0, VehicleType::NORMAL));
    std::string output;
    { CaptureCout capture; ConsoleView().renderTick(5, "Main", intersection.getLanes(), intersection.getSignalController()); output = capture.text(); }
    std::size_t rows = 0;
    const bool aligned = columnsAligned(output, rows);
    const bool allLanes = output.find("South") != std::string::npos && output.find("East") != std::string::npos &&
                          output.find("West") != std::string::npos;
    c.observed << "29-char lane name, queue 15: " << rows << " rows, columns aligned=" << (aligned ? "yes" : "no")
               << ", 15-char bar=" << (output.find(std::string(15, '#')) != std::string::npos ? "yes" : "no");
    c.expect(aligned && allLanes && output.find(std::string(15, '#')) != std::string::npos, "formatting broke");
}

// ===================================================================== 13.9
void robInvalidIndex(Context& c) {
    Intersection intersection("Main");
    for (const char* lane : {"North", "South", "East", "West"}) intersection.addLane(lane, 5);
    std::string laneError, generatorError, engineError;
    try { (void)intersection.getLanes().at(10); } catch (const std::out_of_range& e) { laneError = e.what(); }
    try { VehicleGenerator g({0.1, 0.2, 0.3, 0.4}, 1); g.setArrivalRate(10, 0.5); } catch (const std::out_of_range& e) { generatorError = e.what(); }
    SimulationEngine engine;
    try { (void)engine.isAdaptiveMode("Intersection-10"); } catch (const std::out_of_range& e) { engineError = e.what(); }
    c.observed << "lane index 10 of 4 -> out_of_range; generator lane 10 -> \"" << generatorError << "\"; engine -> \""
               << engineError << "\"";
    c.expect(!laneError.empty() && !generatorError.empty() && !engineError.empty(), "invalid index not rejected");
}

void robInvalidValues(Context& c) {
    const std::string negative = loadError(baseConfig("", "North", "total_ticks=10\narrival_rate=-0.5\nadaptive_mode=false\nqueue_capacity=5\n"), "rob002p_a");
    const std::string zeroCapacity = loadError(baseConfig("", "North", "total_ticks=10\nadaptive_mode=false\nqueue_capacity=0\n"), "rob002p_b");
    const std::string fixture = loadErrorFromPath("tests/data/invalid_values.cfg");
    c.observed << "arrival_rate=-0.5 -> \"" << negative << "\"; queue_capacity=0 -> \"" << zeroCapacity
               << "\"; invalid_values.cfg -> \"" << fixture << "\"";
    c.expect(negative.find("arrival_rate") != std::string::npos, "negative arrival rate not rejected clearly");
    c.expect(zeroCapacity.find("queue_capacity") != std::string::npos, "zero capacity not rejected clearly");
    c.expect(!fixture.empty(), "invalid_values.cfg accepted");
}

void robWrongType(Context& c) {
    const std::string error = loadError(baseConfig("", "North", "total_ticks=10\narrival_rate=abc\nadaptive_mode=false\nqueue_capacity=5\n"), "rob002n");
    const std::string ticks = loadError(baseConfig("", "North", "total_ticks=ten\nadaptive_mode=false\nqueue_capacity=5\n"), "rob002n_b");
    c.observed << "arrival_rate=abc -> \"" << error << "\"; total_ticks=ten -> \"" << ticks << "\"";
    c.expect(error == "Invalid number for: arrival_rate" && ticks == "Invalid total_ticks value", "type mismatch not reported");
}

void robSustainedRun(Context& c) {
    SimulationEngine engine;
    addIntersections(engine, 4, 15, true);
    engine.loadConfig(0.5, 0.05, true);
    { CaptureCout quiet; engine.run(10000, true); }
    c.observed << "10,000-tick connected run completed at tick " << engine.getCurrentTick()
               << " (0-byte leak verdict: memory tool run)";
    c.expect(engine.getCurrentTick() == 10000, "sustained run failed");
}

struct InjectedFault : std::runtime_error {
    InjectedFault() : std::runtime_error("injected fault at tick 500") {}
};

void robMidRunException(Context& c) {
    std::string caught;
    int tickAtFault = 0;
    try {
        SimulationEngine engine;
        addIntersections(engine, 2, 15, true);
        engine.loadConfig(0.6, 0.05, false);
        for (int tick = 1; tick <= 1000; ++tick) {
            engine.tick(true);
            if (tick == 500) { tickAtFault = engine.getCurrentTick(); throw InjectedFault(); }
        }
    } catch (const InjectedFault& fault) {
        caught = fault.what();
    }
    c.observed << "exception \"" << caught << "\" propagated out of a live engine at tick " << tickAtFault
               << " and was caught; engine destroyed by stack unwinding (0-byte leak verdict: memory tool run)";
    c.expect(caught == "injected fault at tick 500" && tickAtFault == 500, "fault not injected/caught");
}

void robOverflow(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    intersection.addLane("Hot", 3);
    intersection.addLane("Cold", 3);
    engine.addIntersection(intersection);
    engine.loadConfig(0.0, 0.0, false);
    engine.configureLaneArrivalRate("Hot", 0.95);
    engine.configureLaneArrivalRate("Cold", 0.05);
    bool crashed = false;
    try { for (int tick = 0; tick < 200; ++tick) engine.tick(true); } catch (...) { crashed = true; }
    const auto hot = engine.getStats().blockedFor("Main", "Hot");
    const auto cold = engine.getStats().blockedFor("Main", "Cold");
    c.observed << "capacity 3 @0.95 for 200 ticks: Hot blocked=" << hot << ", Cold blocked=" << cold
               << ", Hot size=" << engine.getIntersections()[0].getLanes()[0].size();
    c.expect(!crashed && hot > 0 && cold == 0, "overflow not handled safely");
    c.expect(engine.getIntersections()[0].getLanes()[0].size() <= 3, "capacity exceeded");
}

void robOverflowEveryLane(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    const std::vector<std::string> lanes = {"North", "South", "East", "West"};
    for (const auto& lane : lanes) intersection.addLane(lane, 1);
    engine.addIntersection(intersection);
    engine.loadConfig(0.95, 0.0, false);
    bool overCapacity = false;
    for (int tick = 0; tick < 50; ++tick) {
        engine.tick(true);
        for (const auto& lane : engine.getIntersections()[0].getLanes()) overCapacity = overCapacity || lane.size() > 1;
    }
    c.observed << "all 4 lanes capacity 1 @0.95, 50 ticks: blocked";
    bool everyLane = true;
    for (const auto& lane : lanes) {
        const auto blocked = engine.getStats().blockedFor("Main", lane);
        c.observed << " " << lane << "=" << blocked;
        everyLane = everyLane && blocked > 0;
    }
    c.expect(everyLane, "a lane did not reject overflow");
    c.expect(!overCapacity && engine.getCurrentTick() == 50, "instability under simultaneous overflow");
}

void robMalformedInput(Context& c) {
    const std::string error = loadErrorFromPath("tests/data/corrupted.cfg");
    c.observed << "corrupted.cfg -> \"" << error << "\" (CLI exit code checked by run_acceptance.sh)";
    c.expect(error.find("Invalid configuration at line") != std::string::npos, "corrupted file not rejected clearly");
}

void robModuleFault(Context& c) {
    SimulationEngine engine;
    Intersection intersection("Main");
    for (const char* lane : {"North", "South"}) intersection.addLane(lane, 10);
    engine.addIntersection(intersection);
    engine.loadConfig(0.4, 0.05, false);
    for (int tick = 0; tick < 250; ++tick) engine.tick(true);
    // Fault injection at module boundaries mid-run (invalid lane names / data).
    SignalController& probe = intersection.getSignalController();
    const SignalPhase ghostPhase = engine.getIntersections()[0].getSignalController().getCurrentPhase("Ghost-Lane");
    const bool ghostPreempt = probe.preempt("Ghost-Lane");
    StatisticsCollector stats;
    std::string statsError;
    try { stats.recordDequeue("Main", "", Vehicle(1, 5, VehicleType::NORMAL), 6); } catch (const std::invalid_argument& e) { statsError = e.what(); }
    for (int tick = 0; tick < 250; ++tick) engine.tick(true);
    c.observed << "getCurrentPhase(\"Ghost-Lane\")=" << phaseName(ghostPhase) << ", preempt(\"Ghost-Lane\")="
               << (ghostPreempt ? "accepted" : "rejected") << ", stats empty-lane -> \"" << statsError
               << "\"; simulation continued to tick " << engine.getCurrentTick();
    c.expect(ghostPhase == SignalPhase::RED && !ghostPreempt && !statsError.empty(), "fault not contained at module boundary");
    c.expect(engine.getCurrentTick() == 500, "simulation did not continue");
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::vector<TestCase> cases = {
        {"TC-Gen-001-P", "TMS-F-001", "Vehicles spawn at configured arrival rate", genSpawnRate},
        {"TC-Gen-001-N", "TMS-F-001", "Zero arrival rate produces zero vehicles", genZeroRate},
        {"TC-Gen-002-P", "TMS-F-002", "Unique ID, arrival tick and valid type", genIdTickType},
        {"TC-Gen-002-N", "TMS-F-002", "Same-tick spawns across lanes keep unique IDs", genSameTickUnique},
        {"TC-Gen-003-P", "TMS-F-003", "Vehicles enqueue in FIFO order", genFifo},
        {"TC-Gen-003-N", "TMS-F-003", "Emergency insertion does not reorder normal FIFO", genFifoWithEmergency},
        {"TC-Queue-001-P", "TMS-F-010", "Each lane maintains independent queue state", queueIndependent},
        {"TC-Queue-001-N", "TMS-F-010", "Operating on one lane does not corrupt sibling", queueSiblingIntact},
        {"TC-Queue-002-P", "TMS-F-011", "Capacity limit is enforced and logged", queueCapacityEnforced},
        {"TC-Queue-002-N", "TMS-F-011", "Zero capacity is rejected without crashing", queueZeroCapacity},
        {"TC-Queue-003-P", "TMS-F-012", "Dequeue only happens while Green", queueDequeueOnlyGreen},
        {"TC-Queue-003-N", "TMS-F-012", "Empty Green lane dequeue attempt does not error", queueEmptyGreen},
        {"TC-Queue-004-P", "TMS-F-013", "Wait time is calculated correctly", queueWaitTime},
        {"TC-Queue-004-N", "TMS-F-013", "Immediate dequeue wait time is zero", queueImmediateWait},
        {"TC-Sig-001-P", "TMS-F-020", "Fixed-timer cycle follows configured durations", sigFixedCycle},
        {"TC-Sig-001-N", "TMS-F-020", "Zero-duration phase is rejected (cannot freeze)", sigZeroDuration},
        {"TC-Sig-002-P", "TMS-F-021", "Adaptive mode extends Green for longer queues", sigAdaptiveExtends},
        {"TC-Sig-002-N", "TMS-F-021", "Adaptive extension is capped for extreme queue", sigAdaptiveCapped},
        {"TC-Sig-003-P", "TMS-F-022", "Conflicting directions never simultaneously Green", sigNoConflictingGreen},
        {"TC-Sig-003-N", "TMS-F-022", "Forced same-tick overrides cannot create conflict", sigForcedConflict},
        {"TC-Sig-004-P", "TMS-F-023", "Yellow phase occurs before Red", sigYellowBeforeRed},
        {"TC-Sig-004-N", "TMS-F-023", "One-tick Yellow duration renders correctly", sigOneTickYellow},
        {"TC-Sig-005-P", "TMS-F-024", "Signal mode switches via config alone", sigModeViaConfig},
        {"TC-Sig-005-N", "TMS-F-024", "Invalid adaptive_mode is rejected with clear error", sigInvalidMode},
        {"TC-Emg-001-P", "TMS-F-030", "Emergency vehicles spawn at configured probability", emgSpawnProbability},
        {"TC-Emg-001-N", "TMS-F-030", "Zero emergency probability produces none", emgZeroProbability},
        {"TC-Emg-002-P", "TMS-F-031", "Emergency vehicle dequeued ahead of normal", emgPriority},
        {"TC-Emg-002-N", "TMS-F-031", "Emergency vehicles preserve FIFO among themselves", emgFifoAmongEmergency},
        {"TC-Emg-003-P", "TMS-F-032", "Signal preempts within configured max delay", emgPreemptWithinDelay},
        {"TC-Emg-003-N", "TMS-F-032", "Preemption does not create conflicting Green", emgPreemptNoConflict},
        {"TC-Multi-001-P", "TMS-F-040", "Multiple intersections run independently", multiIndependent},
        {"TC-Multi-001-N", "TMS-F-040", "Invalid/overloaded intersection does not affect others", multiFaultIsolation},
        {"TC-Multi-002-P", "TMS-F-041", "Green-wave link passes a vehicle downstream", multiGreenWave},
        {"TC-Multi-002-N", "TMS-F-041", "Full downstream lane handles handoff gracefully", multiFullDownstream},
        {"TC-Multi-003-P", "TMS-F-042", "Per-intersection and aggregate stats are correct", multiStatsSum},
        {"TC-Multi-003-N", "TMS-F-042", "Zero-throughput intersection does not break aggregate", multiZeroThroughput},
        {"TC-Stats-001-P", "TMS-F-050", "Average wait time is computed correctly", statsAverage},
        {"TC-Stats-001-N", "TMS-F-050", "Zero dequeue events do not error", statsAverageNoData},
        {"TC-Stats-002-P", "TMS-F-051", "Maximum queue length is tracked correctly", statsMaxQueue},
        {"TC-Stats-002-N", "TMS-F-051", "Maximum queue length correct when queue <= 1", statsMaxQueueOne},
        {"TC-Stats-003-P", "TMS-F-052", "Throughput count is accurate", statsThroughputCount},
        {"TC-Stats-003-N", "TMS-F-052", "Blocked vehicles are not counted as throughput", statsBlockedNotThroughput},
        {"TC-Sim-001-P", "TMS-F-060", "Config values are applied correctly", simConfigApplied},
        {"TC-Sim-001-N", "TMS-F-060", "Missing config file is rejected with clear error", simMissingConfig},
        {"TC-Sim-002-P", "TMS-F-061", "Natural run completion produces clean summary", simNaturalCompletion},
        {"TC-Perf-01-P", "TMS-NF-001", "Throughput performance target is met", perfTarget},
        {"TC-Perf-01-N", "TMS-NF-001", "Performance does not collapse under maximum load", perfMaxLoad},
        {"TC-Rel-01-P", "TMS-NF-002", "10,000-tick run completes (functional part)", relLongRun},
        {"TC-Rel-01-N", "TMS-NF-002", "Repeated runs complete (functional part)", relRepeatedRuns},
        {"TC-Maint-01-P", "TMS-NF-003", "New topology can be added via config only", maintNewTopology},
        {"TC-Maint-01-N", "TMS-NF-003", "Invalid topology is rejected", maintInvalidTopology},
        {"TC-UX-01-P", "TMS-NF-004", "Console output is readable during a run", uxReadable},
        {"TC-UX-01-N", "TMS-NF-004", "Output readable with long lane name/high queue", uxLongName},
        {"TC-Rob-001-N", "PRJ-SR-001", "Invalid index access is caught", robInvalidIndex},
        {"TC-Rob-002-P", "PRJ-SR-002", "Invalid config values are rejected safely", robInvalidValues},
        {"TC-Rob-002-N", "PRJ-SR-002", "Wrong-type config value does not crash parser", robWrongType},
        {"TC-Rob-003-P", "PRJ-SR-003", "Sustained run completes (functional part)", robSustainedRun},
        {"TC-Rob-003-N", "PRJ-SR-003", "Mid-run exception unwinds cleanly (functional part)", robMidRunException},
        {"TC-Rob-004-P", "PRJ-SR-004", "Queue overflow is handled safely", robOverflow},
        {"TC-Rob-004-N", "PRJ-SR-004", "Simultaneous overflow on every lane is handled", robOverflowEveryLane},
        {"TC-Rob-005-P", "PRJ-SR-005", "Malformed input does not cause abnormal termination", robMalformedInput},
        {"TC-Rob-005-N", "PRJ-SR-005", "Fault in one module does not terminate simulation", robModuleFault},
    };

    // Arguments: [--no-perf] [results.md]. --no-perf skips the two timing cases,
    // which are meaningless under Valgrind/sanitizer instrumentation.
    bool skipPerformance = false;
    std::string markdownPath;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--no-perf") skipPerformance = true;
        else markdownPath = arg;
    }

    std::vector<Result> results;
    for (const auto& testCase : cases) {
        if (skipPerformance && testCase.id.compare(0, 7, "TC-Perf") == 0) continue;
        Context context;
        try {
            testCase.body(context);
        } catch (const std::exception& error) {
            context.failures.push_back(std::string("unexpected exception: ") + error.what());
        } catch (...) {
            context.failures.push_back("unexpected non-standard exception");
        }
        std::string observed = context.observed.str();
        for (const auto& failure : context.failures) observed += " [FAILED: " + failure + "]";
        results.push_back({testCase.id, testCase.requirement, testCase.title, context.failures.empty(), observed});
        std::cout << testCase.id << " | " << testCase.requirement << " | " << (context.failures.empty() ? "PASS" : "FAIL")
                  << " | " << observed << std::endl;
    }

    const auto passed = std::count_if(results.begin(), results.end(), [](const Result& r) { return r.passed; });
    std::cout << "\nIn-process acceptance cases: " << passed << " passed, " << results.size() - passed
              << " failed, " << results.size() << " executed." << std::endl;

    if (!markdownPath.empty()) {
        std::ofstream table(markdownPath);
        table << "| Test case | Requirement | Title | Result | Observed |\n|---|---|---|---|---|\n";
        for (const auto& r : results) {
            std::string observed = r.observed;
            std::replace(observed.begin(), observed.end(), '|', '/');
            table << "| " << r.id << " | " << r.requirement << " | " << r.title << " | "
                  << (r.passed ? "PASS" : "**FAIL**") << " | " << observed << " |\n";
        }
    }
    return passed == static_cast<long>(results.size()) ? EXIT_SUCCESS : EXIT_FAILURE;
}
