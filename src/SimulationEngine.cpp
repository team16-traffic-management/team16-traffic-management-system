#include "../include/SimulationEngine.h"

#include <atomic>
#include <csignal>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace {
// Lock-free atomic flag so a signal handler can request a stop safely.
static_assert(std::atomic<bool>::is_always_lock_free,
              "Graceful signal handling requires lock-free atomic<bool>");
std::atomic<bool> stopRequested{false};
void handleStopSignal(int) { requestSimulationStop(); }
}

void installSimulationSignalHandlers() {
    std::signal(SIGINT, handleStopSignal);
}

void requestSimulationStop() noexcept {
    stopRequested.store(true, std::memory_order_relaxed);
}

SimulationEngine::SimulationEngine() : generator_(0.4, 0.05) {}

void SimulationEngine::loadConfig(double arrivalRate, double emergencyProb, bool adaptiveMode) {
    generator_ = VehicleGenerator(arrivalRate, emergencyProb);
    pendingTransfers_.clear();
    totalTransfers_ = 0;
    for (auto& intersection : intersections_) {
        intersection.getSignalController().setAdaptiveMode(adaptiveMode);
        for (const auto& lane : intersection.getLanes()) {
            generator_.configureLane(intersection.getId(), lane.name(), arrivalRate);
        }
    }
}

void SimulationEngine::addIntersection(const Intersection& intersection) {
    if (intersection.getId().empty()) throw std::invalid_argument("Intersection ID cannot be empty");
    for (const auto& existing : intersections_) {
        if (existing.getId() == intersection.getId())
            throw std::invalid_argument("Duplicate intersection ID: " + intersection.getId());
    }
    intersections_.push_back(intersection);
    const Intersection& added = intersections_.back();
    for (const auto& lane : added.getLanes())
        generator_.configureLane(added.getId(), lane.name(), 0.4);
}

void SimulationEngine::configureLaneArrivalRate(const std::string& laneName, double arrivalRate) {
    if (laneName.empty() || !std::isfinite(arrivalRate) || arrivalRate < 0.0 || arrivalRate > 1.0)
        throw std::invalid_argument("Invalid lane name or arrival rate");
    bool laneFound = false;
    for (const auto& intersection : intersections_) {
        for (const auto& lane : intersection.getLanes()) {
            if (lane.name() == laneName) {
                generator_.configureLane(intersection.getId(), laneName, arrivalRate);
                laneFound = true;
            }
        }
    }
    if (!laneFound) generator_.configureLane(laneName, arrivalRate);
}

void SimulationEngine::connectIntersections(const std::string& sourceId, const std::string& destinationId) {
    if (sourceId.empty() || destinationId.empty())
        throw std::invalid_argument("Intersection IDs cannot be empty");
    if (sourceId == destinationId)
        throw std::invalid_argument("An intersection cannot connect to itself");
    const Intersection* source = nullptr;
    const Intersection* destination = nullptr;
    for (const auto& intersection : intersections_) {
        if (intersection.getId() == sourceId) source = &intersection;
        if (intersection.getId() == destinationId) destination = &intersection;
    }
    if (!source || !destination)
        throw std::invalid_argument("Source or destination intersection does not exist");
    for (const auto& sourceLane : source->getLanes()) {
        bool found = false;
        for (const auto& destinationLane : destination->getLanes()) {
            if (sourceLane.name() == destinationLane.name()) { found = true; break; }
        }
        if (!found)
            throw std::invalid_argument("Destination intersection is missing lane: " + sourceLane.name());
        for (const auto& connection : connections_) {
            if (connection.sourceIntersectionId == sourceId && connection.laneName == sourceLane.name())
                throw std::invalid_argument("This source lane already has an outgoing connection");
        }
    }
    for (const auto& sourceLane : source->getLanes())
        connections_.push_back({sourceId, destinationId, sourceLane.name()});
}

int SimulationEngine::loadConfigFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Could not open configuration file: " + filename);

    std::unordered_map<std::string, std::string> settings;
    std::string line;
    int lineNumber = 0;
    const auto trim = [](std::string& text) {
        const std::size_t first = text.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) { text.clear(); return; }
        const std::size_t last = text.find_last_not_of(" \t\r\n");
        text = text.substr(first, last - first + 1);
    };
    while (std::getline(file, line)) {
        ++lineNumber;
        trim(line);
        if (line.empty() || line[0] == '#') continue;
        const std::size_t separator = line.find('=');
        if (separator == std::string::npos)
            throw std::runtime_error("Invalid configuration at line " + std::to_string(lineNumber));
        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator + 1);
        trim(key); trim(value);
        if (key.empty() || value.empty())
            throw std::runtime_error("Empty key or value at line " + std::to_string(lineNumber));
        if (settings.count(key)) throw std::runtime_error("Duplicate configuration key: " + key);
        settings[key] = value;
    }

    const auto getRequired = [&settings](const std::string& key) -> std::string {
        const auto it = settings.find(key);
        if (it == settings.end()) throw std::runtime_error("Missing configuration key: " + key);
        return it->second;
    };
    const auto parseProbability = [&settings, &getRequired](const std::string& key, double fallback) -> double {
        const auto it = settings.find(key);
        if (it == settings.end()) return fallback;
        const std::string value = it->second;
        std::size_t parsed = 0;
        double result = 0.0;
        try { result = std::stod(value, &parsed); }
        catch (...) { throw std::runtime_error("Invalid number for: " + key); }
        if (parsed != value.size() || !std::isfinite(result) || result < 0.0 || result > 1.0)
            throw std::runtime_error("Value must be a finite number between 0 and 1: " + key);
        return result;
    };

    const std::string ticksText = getRequired("total_ticks");
    std::size_t parsed = 0;
    int totalTicks = 0;
    try { totalTicks = std::stoi(ticksText, &parsed); }
    catch (...) { throw std::runtime_error("Invalid total_ticks value"); }
    if (parsed != ticksText.size() || totalTicks < 0 || totalTicks > 10000000)
        throw std::runtime_error("total_ticks must be between 0 and 10000000 (0 means run until stopped)");

    const std::string adaptiveText = getRequired("adaptive_mode");
    bool adaptiveMode = false;
    if (adaptiveText == "true") adaptiveMode = true;
    else if (adaptiveText != "false") throw std::runtime_error("adaptive_mode must be true or false");

    const std::string capacityText = getRequired("queue_capacity");
    std::size_t capacityParsed = 0;
    int queueCapacity = 0;
    try { queueCapacity = std::stoi(capacityText, &capacityParsed); }
    catch (...) { throw std::runtime_error("Invalid queue_capacity value"); }
    if (capacityParsed != capacityText.size() || queueCapacity <= 0 || queueCapacity > 10000)
        throw std::runtime_error("queue_capacity must be between 1 and 10000");

    const double arrivalRate = parseProbability("arrival_rate", 0.4);
    const double emergencyProbability = parseProbability("emergency_probability", 0.05);

    std::map<std::string, std::vector<std::string>> configuredIntersections;
    std::vector<std::pair<std::string, std::string>> configuredConnections;
    const std::string prefix = "intersection.";
    for (const auto& entry : settings) {
        if (entry.first.compare(0, prefix.size(), prefix) == 0) {
            const std::string remainder = entry.first.substr(prefix.size());
            const std::string marker = ".lanes";
            const std::size_t markerPos = remainder.rfind(marker);
            if (markerPos == std::string::npos || markerPos + marker.size() != remainder.size())
                throw std::runtime_error("Unknown intersection setting: " + entry.first);
            const std::string id = remainder.substr(0, markerPos);
            if (id.empty()) throw std::runtime_error("Intersection ID cannot be empty");
            std::vector<std::string> lanes;
            std::string laneList = entry.second;
            std::size_t start = 0;
            while (start <= laneList.size()) {
                const std::size_t comma = laneList.find(',', start);
                std::string lane = laneList.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
                trim(lane);
                if (lane.empty()) throw std::runtime_error("Empty lane name in intersection: " + id);
                lanes.push_back(lane);
                if (comma == std::string::npos) break;
                start = comma + 1;
            }
            if (!configuredIntersections.emplace(id, lanes).second)
                throw std::runtime_error("Duplicate intersection definition: " + id);
        } else if (entry.first.compare(0, 11, "connection.") == 0) {
            const std::string source = entry.first.substr(11);
            if (source.empty() || entry.second.empty())
                throw std::runtime_error("Connection source and destination must be specified");
            configuredConnections.push_back({source, entry.second});
        } else {
            const std::string allowed[] = {"total_ticks", "arrival_rate", "emergency_probability",
                "adaptive_mode", "queue_capacity", "North_Bound", "South_Bound", "East_Bound", "West_Bound"};
            bool known = false;
            for (const auto& key : allowed) if (entry.first == key) { known = true; break; }
            if (!known) throw std::runtime_error("Unknown configuration key: " + entry.first);
        }
    }

    if (!configuredIntersections.empty()) {
        intersections_.clear();
        connections_.clear();
        for (const auto& definition : configuredIntersections) {
            Intersection intersection(definition.first);
            for (const auto& lane : definition.second)
                intersection.addLane(lane, static_cast<std::size_t>(queueCapacity));
            addIntersection(intersection);
        }
    }

    configuredQueueCapacity_ = queueCapacity;
    loadConfig(arrivalRate, emergencyProbability, adaptiveMode);

    // Legacy four-lane rate keys remain supported for existing configuration files.
    const char* legacyLanes[] = {"North_Bound", "South_Bound", "East_Bound", "West_Bound"};
    for (const char* lane : legacyLanes) {
        const auto it = settings.find(lane);
        if (it != settings.end()) configureLaneArrivalRate(lane, parseProbability(lane, arrivalRate));
    }
    for (const auto& connection : configuredConnections) {
        try {
            connectIntersections(connection.first, connection.second);
        } catch (const std::invalid_argument& error) {
            throw std::runtime_error(std::string("Invalid configured connection: ") + error.what());
        }
    }

    std::cout << "\nConfiguration loaded successfully from " << filename
              << " (" << intersections_.size() << " intersection(s))\n";
    return totalTicks;
}

void SimulationEngine::deliverPendingTransfers() {
    std::deque<PendingTransfer> remaining;
    while (!pendingTransfers_.empty()) {
        PendingTransfer transfer = pendingTransfers_.front();
        pendingTransfers_.pop_front();
        bool delivered = false;
        for (auto& destination : intersections_) {
            if (destination.getId() != transfer.destinationIntersectionId) continue;
            for (auto& lane : destination.getLanes()) {
                if (lane.name() != transfer.laneName) continue;
                if (lane.enqueue(transfer.vehicle)) {
                    ++totalTransfers_;
                    delivered = true;
                    stats_.updateMaxQueueLength(destination.getId(), lane.name(), lane.size());
                }
                break;
            }
            break;
        }
        if (!delivered) remaining.push_back(transfer);
    }
    pendingTransfers_.swap(remaining);
}

void SimulationEngine::tick(bool silent) {
    if (currentTick_ == std::numeric_limits<int>::max())
        throw std::overflow_error("Simulation tick limit reached");
    ++currentTick_;
    deliverPendingTransfers();
    for (auto& intersection : intersections_) {
        const std::string intersectionId = intersection.getId();
        for (auto& lane : intersection.getLanes()) {
            const std::string laneName = lane.name();
            if (generator_.shouldGenerate(intersectionId, laneName)) {
                const Vehicle vehicle = generator_.generateVehicle(intersectionId, laneName, currentTick_);
                if (!lane.enqueue(vehicle)) stats_.recordBlocked(intersectionId, laneName, currentTick_);
            }
            stats_.updateMaxQueueLength(intersectionId, laneName, lane.size());
        }
        for (auto& lane : intersection.getLanes()) {
            const std::string laneName = lane.name();
            if (lane.hasEmergency() && intersection.getSignalController().preempt(laneName)) {
                stats_.recordPreemption(intersectionId, laneName, currentTick_);
                break;
            }
        }
        intersection.update(currentTick_);
        if (intersection.getSignalController().isAdaptiveMode()) {
            std::map<std::string, std::size_t> queueSizes;
            for (const auto& lane : intersection.getLanes()) queueSizes[lane.name()] = lane.size();
            intersection.getSignalController().updateAdaptiveTimers(queueSizes);
        }
        for (auto& lane : intersection.getLanes()) {
            const std::string laneName = lane.name();
            if (intersection.getSignalController().getCurrentPhase(laneName) == SignalPhase::GREEN &&
                !lane.isEmpty()) {
                const Vehicle vehicle = lane.dequeue();
                bool scheduled = false;
                for (const auto& connection : connections_) {
                    if (connection.sourceIntersectionId == intersectionId && connection.laneName == laneName) {
                        pendingTransfers_.push_back({connection.destinationIntersectionId, connection.laneName, vehicle});
                        scheduled = true;
                        break;
                    }
                }
                if (!scheduled) stats_.recordDequeue(intersectionId, laneName, vehicle, currentTick_);
            }
        }
        view_.renderTick(currentTick_, intersectionId, intersection.getLanes(),
                         intersection.getSignalController(), silent);
    }
}

void SimulationEngine::run(int maxTicks, bool silent) {
    if (maxTicks < 0)
        throw std::invalid_argument("Simulation ticks cannot be negative");
    if (maxTicks > 0 && maxTicks > std::numeric_limits<int>::max() - currentTick_)
        throw std::overflow_error("Requested simulation duration exceeds tick limit");

    // A zero limit means an open-ended run. SIGINT (Ctrl+C) or an explicit
    // requestSimulationStop() call ends the loop between completed ticks.
    stopRequested.store(false, std::memory_order_relaxed);
    int ticksExecuted = 0;
    while ((maxTicks == 0 || ticksExecuted < maxTicks) &&
           !stopRequested.load(std::memory_order_relaxed)) {
        tick(silent);
        ++ticksExecuted;
    }

    const bool userStopped = stopRequested.load(std::memory_order_relaxed);
    stats_.printEndOfRunReport(ticksExecuted);
    std::cout << "Run termination: "
              << (userStopped ? "user requested stop" : "configured tick limit reached")
              << " (" << ticksExecuted << " ticks executed)\n";
    std::cout << "Total successful vehicle transfers: " << totalTransfers_ << "\n";
    std::cout << "Vehicles still waiting for transfer: " << pendingTransfers_.size() << "\n";
}

const StatisticsCollector& SimulationEngine::getStats() const { return stats_; }
int SimulationEngine::getConfiguredQueueCapacity() const { return configuredQueueCapacity_; }
std::size_t SimulationEngine::getIntersectionCount() const { return intersections_.size(); }
bool SimulationEngine::isAdaptiveMode(const std::string& intersectionId) const {
    for (const auto& intersection : intersections_) {
        if (intersection.getId() == intersectionId)
            return intersection.getSignalController().isAdaptiveMode();
    }
    throw std::out_of_range("Unknown intersection ID: " + intersectionId);
}
std::size_t SimulationEngine::getTotalTransfers() const { return totalTransfers_; }
int SimulationEngine::getCurrentTick() const { return currentTick_; }
