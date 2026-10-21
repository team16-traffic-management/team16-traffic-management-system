
#include "StatisticsCollector.h"

#include <iostream>

void StatisticsCollector::recordDequeue(
    const std::string& laneName,
    const Vehicle& vehicle,
    int dequeueTick
) {
    // Ignore invalid dequeue ticks.
    if (dequeueTick < 0) {
        return;
    }

    // A vehicle cannot leave before it arrives.
    if (dequeueTick < vehicle.arrivalTick) {
        return;
    }

    events_.push_back({laneName, vehicle, dequeueTick});
}

void StatisticsCollector::recordQueueLength(
    const std::string& laneName,
    int queueLength
) {
    // Ignore invalid queue lengths.
    if (queueLength < 0) {
        return;
    }

    auto it = maximumQueueLengths_.find(laneName);

    if (it == maximumQueueLengths_.end()) {
        maximumQueueLengths_[laneName] = queueLength;
    } else if (queueLength > it->second) {
        it->second = queueLength;
    }
}

double StatisticsCollector::averageWaitTime() const {
    if (events_.empty()) {
        return 0.0;
    }

    long long totalWait = 0;

    for (const DequeueEvent& event : events_) {
        totalWait += event.dequeueTick - event.vehicle.arrivalTick;
    }

    return static_cast<double>(totalWait) / events_.size();
}

double StatisticsCollector::averageWaitTimeForLane(
    const std::string& laneName
) const {
    long long totalWait = 0;
    int vehicleCount = 0;

    for (const DequeueEvent& event : events_) {
        if (event.laneName == laneName) {
            totalWait +=
                event.dequeueTick - event.vehicle.arrivalTick;
            ++vehicleCount;
        }
    }

    if (vehicleCount == 0) {
        return 0.0;
    }

    return static_cast<double>(totalWait) / vehicleCount;
}

int StatisticsCollector::totalThroughput() const {
    return static_cast<int>(events_.size());
}

int StatisticsCollector::maximumQueueLengthForLane(
    const std::string& laneName
) const {
    const auto it = maximumQueueLengths_.find(laneName);

    if (it == maximumQueueLengths_.end()) {
        return 0;
    }

    return it->second;
}

void StatisticsCollector::printEndOfRunReport() const {
    std::cout << "\n========== SIMULATION SUMMARY ==========\n";

    std::cout << "Total vehicles cleared: "
              << totalThroughput() << '\n';

    std::cout << "Average waiting time: "
              << averageWaitTime() << " ticks\n";

    std::cout << "\nPer-lane statistics:\n";

    for (const auto& entry : maximumQueueLengths_) {
        const std::string& laneName = entry.first;

        std::cout << "Lane: " << laneName << '\n';

        std::cout << "  Average waiting time: "
                  << averageWaitTimeForLane(laneName)
                  << " ticks\n";

        std::cout << "  Maximum queue length: "
                  << maximumQueueLengthForLane(laneName)
                  << '\n';
    }

    std::cout << "========================================\n";
}
