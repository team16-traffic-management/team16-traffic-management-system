
#include "StatisticsCollector.h"

#include <iomanip>
#include <stdexcept>
#include <sstream>

std::string StatisticsCollector::makeLaneKey(
    const std::string& intersectionId,
    const std::string& laneName
) {
    if (intersectionId.empty() || laneName.empty()) {
        throw std::invalid_argument(
            "Intersection ID and lane name cannot be empty"
        );
    }

    return intersectionId + "::" + laneName;
}

void StatisticsCollector::recordDequeue(
    const std::string& intersectionId,
    const std::string& laneName,
    const Vehicle& vehicle,
    int dequeueTick
) {
    if (dequeueTick < vehicle.getArrivalTick()) {
        throw std::invalid_argument(
            "Dequeue tick cannot be earlier than arrival tick"
        );
    }

    const int wait =
        dequeueTick - vehicle.getArrivalTick();

    const std::string key =
        makeLaneKey(intersectionId, laneName);

    events_.push_back({
        intersectionId,
        laneName,
        vehicle.getId(),
        wait,
        dequeueTick,
        vehicle.isEmergency()
    });

    perLaneThroughput_[key]++;
    perLaneWaitTime_[key] += wait;
}

void StatisticsCollector::recordBlocked(
    const std::string& intersectionId,
    const std::string& laneName,
    int tick
) {
    if (tick < 0) {
        throw std::invalid_argument(
            "Tick cannot be negative"
        );
    }

    blockedEvents_[makeLaneKey(intersectionId, laneName)]++;
}

void StatisticsCollector::recordPreemption(
    const std::string& intersectionId,
    const std::string& laneName,
    int tick
) {
    if (tick < 0) {
        throw std::invalid_argument(
            "Tick cannot be negative"
        );
    }

    preemptionEvents_[makeLaneKey(intersectionId, laneName)]++;
}

void StatisticsCollector::updateMaxQueueLength(
    const std::string& intersectionId,
    const std::string& laneName,
    std::size_t currentLength
) {
    const std::string key =
        makeLaneKey(intersectionId, laneName);

    if (currentLength > maxQueueLengths_[key]) {
        maxQueueLengths_[key] = currentLength;
    }
}

// Backward-compatible methods use a default intersection label.

void StatisticsCollector::recordDequeue(
    const std::string& laneName,
    const Vehicle& vehicle,
    int dequeueTick
) {
    recordDequeue("Default", laneName, vehicle, dequeueTick);
}

void StatisticsCollector::recordBlocked(
    const std::string& laneName,
    int tick
) {
    recordBlocked("Default", laneName, tick);
}

void StatisticsCollector::recordPreemption(
    const std::string& laneName,
    int tick
) {
    recordPreemption("Default", laneName, tick);
}

void StatisticsCollector::updateMaxQueueLength(
    const std::string& laneName,
    std::size_t currentLength
) {
    updateMaxQueueLength("Default", laneName, currentLength);
}

double StatisticsCollector::averageWaitTime() const {
    if (events_.empty()) {
        return 0.0;
    }

    long long totalWait = 0;

    for (const auto& event : events_) {
        totalWait += event.waitTime;
    }

    return static_cast<double>(totalWait) /
           static_cast<double>(events_.size());
}

std::int64_t StatisticsCollector::totalThroughput() const {
    return static_cast<std::int64_t>(events_.size());
}

std::int64_t StatisticsCollector::totalBlocked() const {
    std::int64_t total = 0;

    for (const auto& entry : blockedEvents_) {
        total += entry.second;
    }

    return total;
}

std::int64_t StatisticsCollector::totalPreemptions() const {
    std::int64_t total = 0;

    for (const auto& entry : preemptionEvents_) {
        total += entry.second;
    }

    return total;
}

void StatisticsCollector::printEndOfRunReport(int runTicks) const {
    std::cout
        << "\n==================================================\n"
        << "         TMS SIMULATION REPORT ("
        << runTicks << " TICKS)\n"
        << "==================================================\n";

    std::cout
        << "Total Throughput (Processed)        : "
        << totalThroughput() << "\n";

    std::cout
        << "Overall Average Wait Time           : "
        << std::fixed << std::setprecision(2)
        << averageWaitTime() << " ticks\n";

    std::cout
        << "Total Overflow (Blocked) Vehicles   : "
        << totalBlocked() << "\n";

    std::cout
        << "Total Emergency Preemption Events   : "
        << totalPreemptions() << "\n\n";

    std::cout << "Per-Intersection Lane Breakdown:\n";

    std::cout
        << "--------------------------------------------------------------------------\n";

    std::cout
        << std::left
        << std::setw(18) << "Intersection"
        << std::setw(17) << "Lane"
        << std::setw(12) << "Throughput"
        << std::setw(16) << "Avg Wait"
        << std::setw(12) << "Max Queue"
        << "\n";

    std::cout
        << "--------------------------------------------------------------------------\n";

    for (const auto& entry : maxQueueLengths_) {
        const std::string& key = entry.first;

        const std::size_t separator =
            key.find("::");

        const std::string intersectionId =
            key.substr(0, separator);

        const std::string laneName =
            key.substr(separator + 2);

        const std::int64_t throughput =
            perLaneThroughput_.count(key)
                ? perLaneThroughput_.at(key)
                : 0;

        const double averageWait =
            throughput > 0
                ? static_cast<double>(perLaneWaitTime_.at(key)) /
                  static_cast<double>(throughput)
                : 0.0;

        std::cout
            << std::left
            << std::setw(18) << intersectionId
            << std::setw(17) << laneName
            << std::setw(12) << throughput
            << std::setw(16) << std::fixed
            << std::setprecision(2) << averageWait
            << std::setw(12) << entry.second
            << "\n";
    }

    std::cout
        << "==========================================================================\n\n";
}
