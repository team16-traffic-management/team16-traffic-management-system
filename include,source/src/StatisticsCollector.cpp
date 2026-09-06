#include "StatisticsCollector.h"
#include <algorithm>
#include <iomanip>
#include <iostream>

void StatisticsCollector::recordDequeue(const std::string& laneName,
                                         const Vehicle& v,
                                         int dequeueTick) {
    events_.push_back({laneName, v, dequeueTick});
}

double StatisticsCollector::averageWaitTime() const {
    if (events_.empty()) return 0.0;
    double total = 0.0;
    for (const auto& e : events_) {
        total += (e.dequeueTick - e.vehicle.arrivalTick);
    }
    return total / static_cast<double>(events_.size());
}

double StatisticsCollector::averageWaitTimeForLane(const std::string& laneName) const {
    double total = 0.0;
    int count = 0;
    for (const auto& e : events_) {
        if (e.laneName == laneName) {
            total += (e.dequeueTick - e.vehicle.arrivalTick);
            count++;
        }
    }
    return count == 0 ? 0.0 : total / count;
}

int StatisticsCollector::totalThroughput() const {
    return static_cast<int>(events_.size());
}

void StatisticsCollector::printEndOfRunReport() const {
    std::cout << "\n===== SIMULATION SUMMARY =====\n";
    std::cout << "Total vehicles cleared : " << totalThroughput() << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Overall avg wait time  : " << averageWaitTime() << " ticks\n";

    std::vector<std::string> lanesSeen;
    for (const auto& e : events_) {
        if (std::find(lanesSeen.begin(), lanesSeen.end(), e.laneName) == lanesSeen.end()) {
            lanesSeen.push_back(e.laneName);
        }
    }
    for (const auto& lane : lanesSeen) {
        std::cout << "  " << std::left << std::setw(10) << lane
                  << ": avg wait " << averageWaitTimeForLane(lane) << " ticks\n";
    }
    std::cout << "===============================\n";
}
