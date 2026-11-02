#include "SimulationEngine.h"

#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cctype>
#include <cmath>
#include <limits>
#include <set>
#include <deque>

// --------------------------------------------------
// CONSTRUCTOR
// --------------------------------------------------

SimulationEngine::SimulationEngine()
    : generator_(0.4, 0.05) {}

// --------------------------------------------------
// CONFIGURATION
// --------------------------------------------------

void SimulationEngine::loadConfig(
    double arrivalRate,
    double emergencyProb,
    bool adaptiveMode
) {
    generator_ = VehicleGenerator(arrivalRate, emergencyProb);

    // Clear transfers from any previous simulation configuration.
    pendingTransfers_.clear();
    totalTransfers_ = 0;

    for (auto& inter : intersections_) {
        inter.getSignalController().setAdaptiveMode(adaptiveMode);

        for (const auto& lane : inter.getLanes()) {
            generator_.configureLane(
                inter.getId(),
                lane.name(),
                arrivalRate
            );
        }
    }
}

void SimulationEngine::addIntersection(
    const Intersection& intersection
) {
    if (intersection.getId().empty()) {
        throw std::invalid_argument(
            "Intersection ID cannot be empty"
        );
    }

    for (const auto& existing : intersections_) {
        if (existing.getId() == intersection.getId()) {
            throw std::invalid_argument(
                "Duplicate intersection ID: " + intersection.getId()
            );
        }
    }

    intersections_.push_back(intersection);

    // Initialize independent random states for its lanes.
    const Intersection& added = intersections_.back();

    for (const auto& lane : added.getLanes()) {
        generator_.configureLane(
            added.getId(),
            lane.name(),
            0.4
        );
    }
}

void SimulationEngine::configureLaneArrivalRate(
    const std::string& laneName,
    double arrivalRate
) {
    if (laneName.empty() ||
        !std::isfinite(arrivalRate) ||
        arrivalRate < 0.0 ||
        arrivalRate > 1.0) {

        throw std::invalid_argument(
            "Invalid lane name or arrival rate"
        );
    }

    bool laneFound = false;

    for (const auto& inter : intersections_) {
        for (const auto& lane : inter.getLanes()) {
            if (lane.name() == laneName) {
                generator_.configureLane(
                    inter.getId(),
                    laneName,
                    arrivalRate
                );

                laneFound = true;
            }
        }
    }

    // Preserve compatibility if configuration is loaded
    // before intersections have been added.
    if (!laneFound) {
        generator_.configureLane(laneName, arrivalRate);
    }
}

// --------------------------------------------------
// CONNECT INTERSECTIONS
// --------------------------------------------------

void SimulationEngine::connectIntersections(
    const std::string& sourceIntersectionId,
    const std::string& destinationIntersectionId
) {
    if (sourceIntersectionId.empty() ||
        destinationIntersectionId.empty()) {

        throw std::invalid_argument(
            "Intersection IDs cannot be empty"
        );
    }

    if (sourceIntersectionId == destinationIntersectionId) {
        throw std::invalid_argument(
            "An intersection cannot connect to itself"
        );
    }

    const Intersection* source = nullptr;
    const Intersection* destination = nullptr;

    for (const auto& intersection : intersections_) {
        if (intersection.getId() == sourceIntersectionId) {
            source = &intersection;
        }

        if (intersection.getId() == destinationIntersectionId) {
            destination = &intersection;
        }
    }

    if (!source || !destination) {
        throw std::invalid_argument(
            "Source or destination intersection does not exist"
        );
    }

    // Validate all lanes before registering connections.
    for (const auto& sourceLane : source->getLanes()) {

        bool matchingLaneFound = false;

        for (const auto& destinationLane : destination->getLanes()) {
            if (sourceLane.name() == destinationLane.name()) {
                matchingLaneFound = true;
                break;
            }
        }

        if (!matchingLaneFound) {
            throw std::invalid_argument(
                "Destination intersection is missing lane: " +
                sourceLane.name()
            );
        }

        // A source lane can have only one outgoing destination.
        for (const auto& connection : connections_) {

            if (
                connection.sourceIntersectionId == sourceIntersectionId &&
                connection.laneName == sourceLane.name()
            ) {
                throw std::invalid_argument(
                    "This source lane already has an outgoing connection"
                );
            }
        }
    }

    // Register matching source and destination lanes.
    for (const auto& sourceLane : source->getLanes()) {

        connections_.push_back({
            sourceIntersectionId,
            destinationIntersectionId,
            sourceLane.name()
        });
    }

    std::cout
        << "Connected intersection "
        << sourceIntersectionId
        << " to "
        << destinationIntersectionId
        << " successfully.\n";
}

// --------------------------------------------------
// LOAD CONFIGURATION FILE
// --------------------------------------------------

int SimulationEngine::loadConfigFromFile(
    const std::string& filename
) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open configuration file: " + filename
        );
    }

    std::unordered_map<std::string, std::string> settings;
    std::string line;
    int lineNumber = 0;

    auto trim = [](std::string& text) {

        const std::size_t first =
            text.find_first_not_of(" \t\r\n");

        if (first == std::string::npos) {
            text.clear();
            return;
        }

        const std::size_t last =
            text.find_last_not_of(" \t\r\n");

        text = text.substr(first, last - first + 1);
    };

    while (std::getline(file, line)) {

        ++lineNumber;

        trim(line);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t separator = line.find('=');

        if (separator == std::string::npos) {
            throw std::runtime_error(
                "Invalid configuration at line " +
                std::to_string(lineNumber)
            );
        }

        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator + 1);

        trim(key);
        trim(value);

        if (key.empty() || value.empty()) {
            throw std::runtime_error(
                "Empty key or value at line " +
                std::to_string(lineNumber)
            );
        }

        if (settings.count(key)) {
            throw std::runtime_error(
                "Duplicate configuration key: " + key
            );
        }

        settings[key] = value;
    }

    const std::string allowedKeys[] = {
        "total_ticks",
        "emergency_probability",
        "adaptive_mode",
        "North_Bound",
        "South_Bound",
        "East_Bound",
        "West_Bound",
        "queue_capacity"
    };

    for (const auto& setting : settings) {

        bool validKey = false;

        for (const auto& allowed : allowedKeys) {
            if (setting.first == allowed) {
                validKey = true;
                break;
            }
        }

        if (!validKey) {
            throw std::runtime_error(
                "Unknown configuration key: " + setting.first
            );
        }
    }

    auto getValue = [&](const std::string& key) -> std::string {

        const auto it = settings.find(key);

        if (it == settings.end()) {
            throw std::runtime_error(
                "Missing configuration key: " + key
            );
        }

        return it->second;
    };

    auto parseProbability =
        [&](const std::string& key) -> double {

        const std::string value = getValue(key);

        std::size_t parsed = 0;
        double result;

        try {
            result = std::stod(value, &parsed);
        }
        catch (...) {
            throw std::runtime_error(
                "Invalid number for: " + key
            );
        }

        if (parsed != value.size() ||
            !std::isfinite(result) ||
            result < 0.0 ||
            result > 1.0) {

            throw std::runtime_error(
                "Value must be a finite number between 0 and 1: " +
                key
            );
        }

        return result;
    };

    // Validate total simulation ticks.
    const std::string ticksText = getValue("total_ticks");

    std::size_t parsed = 0;
    int totalTicks;

    try {
        totalTicks = std::stoi(ticksText, &parsed);
    }
    catch (...) {
        throw std::runtime_error(
            "Invalid total_ticks value"
        );
    }

    if (parsed != ticksText.size() ||
        totalTicks <= 0 ||
        totalTicks > 10000000) {

        throw std::runtime_error(
            "total_ticks must be between 1 and 10000000"
        );
    }

    // Validate adaptive mode.
    const std::string adaptiveText =
        getValue("adaptive_mode");

    bool adaptiveMode;

    if (adaptiveText == "true") {
        adaptiveMode = true;
    }
    else if (adaptiveText == "false") {
        adaptiveMode = false;
    }
    else {
        throw std::runtime_error(
            "adaptive_mode must be true or false"
        );
    }

    // Validate queue capacity.
    const std::string capacityText =
        getValue("queue_capacity");

    std::size_t capacityParsed = 0;
    int queueCapacity;

    try {
        queueCapacity = std::stoi(
            capacityText,
            &capacityParsed
        );
    }
    catch (...) {
        throw std::runtime_error(
            "Invalid queue_capacity value"
        );
    }

    if (capacityParsed != capacityText.size() ||
        queueCapacity <= 0 ||
        queueCapacity > 10000) {

        throw std::runtime_error(
            "queue_capacity must be between 1 and 10000"
        );
    }

    configuredQueueCapacity_ = queueCapacity;

    // Read configured probabilities.
    const double emergencyProbability =
        parseProbability("emergency_probability");

    const double northRate =
        parseProbability("North_Bound");

    const double southRate =
        parseProbability("South_Bound");

    const double eastRate =
        parseProbability("East_Bound");

    const double westRate =
        parseProbability("West_Bound");

    // Apply configuration.
    loadConfig(
        0.4,
        emergencyProbability,
        adaptiveMode
    );

    configureLaneArrivalRate("North_Bound", northRate);
    configureLaneArrivalRate("South_Bound", southRate);
    configureLaneArrivalRate("East_Bound", eastRate);
    configureLaneArrivalRate("West_Bound", westRate);

    std::cout
        << "\nConfiguration loaded successfully from "
        << filename << "\n";

    return totalTicks;
}

// --------------------------------------------------
// DELIVER PENDING VEHICLE TRANSFERS
// --------------------------------------------------

void SimulationEngine::deliverPendingTransfers() {

    std::deque<PendingTransfer> remainingTransfers;

    while (!pendingTransfers_.empty()) {

        PendingTransfer transfer = pendingTransfers_.front();
        pendingTransfers_.pop_front();

        bool delivered = false;

        // Find the destination intersection.
        for (auto& destination : intersections_) {

            if (
                destination.getId() !=
                transfer.destinationIntersectionId
            ) {
                continue;
            }

            // Find the matching destination lane.
            for (auto& lane : destination.getLanes()) {

                if (lane.name() != transfer.laneName) {
                    continue;
                }

                // Enqueue returns false when the destination is full.
                if (lane.enqueue(transfer.vehicle)) {

                    ++totalTransfers_;
                    delivered = true;

                    stats_.updateMaxQueueLength(
                        destination.getId(),
                        lane.name(),
                        lane.size()
                    );
                }

                break;
            }

            break;
        }

        // Do not discard vehicles when the destination is full.
        // Keep them pending and retry during the next tick.
        if (!delivered) {
            remainingTransfers.push_back(transfer);
        }
    }

    pendingTransfers_.swap(remainingTransfers);
}

// --------------------------------------------------
// SIMULATION TICK
// --------------------------------------------------

void SimulationEngine::tick(bool silent) {

    if (currentTick_ == std::numeric_limits<int>::max()) {
        throw std::overflow_error(
            "Simulation tick limit reached"
        );
    }

    ++currentTick_;

    // ----------------------------------------------
    // 0. DELIVER VEHICLES FROM PREVIOUS TICKS
    // ----------------------------------------------

    deliverPendingTransfers();

    for (auto& inter : intersections_) {

        const std::string intersectionId = inter.getId();

        // ------------------------------------------
        // 1. GENERATE VEHICLES
        // ------------------------------------------

        for (auto& lane : inter.getLanes()) {

            const std::string laneName = lane.name();

            if (generator_.shouldGenerate(
                    intersectionId,
                    laneName
                )) {

                Vehicle vehicle =
                    generator_.generateVehicle(
                        intersectionId,
                        laneName,
                        currentTick_
                    );

                if (!lane.enqueue(vehicle)) {

                    stats_.recordBlocked(
                        intersectionId,
                        laneName,
                        currentTick_
                    );
                }
            }

            stats_.updateMaxQueueLength(
                intersectionId,
                laneName,
                lane.size()
            );
        }

        // ------------------------------------------
        // 2. EMERGENCY PREEMPTION
        // ------------------------------------------

        for (auto& lane : inter.getLanes()) {

            const std::string laneName = lane.name();

            if (lane.hasEmergency()) {

                const bool accepted =
                    inter.getSignalController().preempt(laneName);

                if (accepted) {

                    stats_.recordPreemption(
                        intersectionId,
                        laneName,
                        currentTick_
                    );

                    break;
                }
            }
        }

        // ------------------------------------------
        // 3. UPDATE TRAFFIC SIGNAL
        // ------------------------------------------

        inter.update(currentTick_);

        // ------------------------------------------
        // 4. ADAPTIVE SIGNAL TIMING
        // ------------------------------------------

        if (inter.getSignalController().isAdaptiveMode()) {

            std::map<std::string, std::size_t> queueSizes;

            for (const auto& lane : inter.getLanes()) {
                queueSizes[lane.name()] = lane.size();
            }

            inter.getSignalController().updateAdaptiveTimers(
                queueSizes
            );
        }

        // ------------------------------------------
        // 5. PROCESS VEHICLES DURING GREEN
        // ------------------------------------------

        for (auto& lane : inter.getLanes()) {

            const std::string laneName = lane.name();

            if (
                inter.getSignalController().getCurrentPhase(laneName)
                    == SignalPhase::GREEN &&
                !lane.isEmpty()
            ) {

                const Vehicle vehicle = lane.dequeue();

                bool transferScheduled = false;

                // Check whether this lane has an outgoing connection.
                for (const auto& connection : connections_) {

                    if (
                        connection.sourceIntersectionId == intersectionId &&
                        connection.laneName == laneName
                    ) {

                        // Stage the vehicle for delivery at the next tick.
                        // It is not counted as completed throughput here.
                        pendingTransfers_.push_back({
                            connection.destinationIntersectionId,
                            connection.laneName,
                            vehicle
                        });

                        transferScheduled = true;
                        break;
                    }
                }

                // Only vehicles leaving an unconnected lane have
                // completed their journey.
                if (!transferScheduled) {

                    stats_.recordDequeue(
                        intersectionId,
                        laneName,
                        vehicle,
                        currentTick_
                    );
                }
            }
        }

        // ------------------------------------------
        // 6. DISPLAY SIMULATION
        // ------------------------------------------

        view_.renderTick(
            currentTick_,
            inter.getLanes(),
            inter.getSignalController(),
            silent
        );
    }
}

// --------------------------------------------------
// RUN SIMULATION
// --------------------------------------------------

void SimulationEngine::run(
    int maxTicks,
    bool silent
) {
    if (maxTicks <= 0) {
        throw std::invalid_argument(
            "Simulation ticks must be greater than zero"
        );
    }

    if (maxTicks >
        std::numeric_limits<int>::max() - currentTick_) {

        throw std::overflow_error(
            "Requested simulation duration exceeds tick limit"
        );
    }

    for (int i = 0; i < maxTicks; ++i) {
        tick(silent);
    }

    stats_.printEndOfRunReport(maxTicks);

    std::cout
        << "\nTotal successful vehicle transfers: "
        << totalTransfers_
        << "\n";

    std::cout
        << "Vehicles still waiting for transfer: "
        << pendingTransfers_.size()
        << "\n";
}

// --------------------------------------------------
// GETTERS
// --------------------------------------------------

const StatisticsCollector& SimulationEngine::getStats() const {
    return stats_;
}

int SimulationEngine::getConfiguredQueueCapacity() const {
    return configuredQueueCapacity_;
}

std::size_t SimulationEngine::getTotalTransfers() const {
    return totalTransfers_;
}