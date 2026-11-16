#ifndef STATISTICS_COLLECTOR_H
#define STATISTICS_COLLECTOR_H

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../vehicle/Vehicle.h"

struct DequeueEvent {
    std::string intersectionId;
    std::string laneName;
    int vehicleId;
    int waitTime;
    int dequeueTick;
    bool isEmergency;
};

struct IntersectionTotals {
    std::int64_t throughput{0};
    std::int64_t totalWaitTime{0};
    std::int64_t blocked{0};
    std::int64_t preemptions{0};
    std::size_t maxQueueLength{0};

    double averageWaitTime() const {
        return throughput > 0
            ? static_cast<double>(totalWaitTime) / static_cast<double>(throughput)
            : 0.0;
    }
};

class StatisticsCollector {
public:
    // Intersection-aware methods
    void recordDequeue(
        const std::string& intersectionId,
        const std::string& laneName,
        const Vehicle& vehicle,
        int dequeueTick
    );

    void recordBlocked(
        const std::string& intersectionId,
        const std::string& laneName,
        int tick
    );

    void recordPreemption(
        const std::string& intersectionId,
        const std::string& laneName,
        int tick
    );

    void updateMaxQueueLength(
        const std::string& intersectionId,
        const std::string& laneName,
        std::size_t currentLength
    );

    // Backward-compatible methods use the default intersection label.
    void recordDequeue(
        const std::string& laneName,
        const Vehicle& vehicle,
        int dequeueTick
    );

    void recordBlocked(const std::string& laneName, int tick);
    void recordPreemption(const std::string& laneName, int tick);

    void updateMaxQueueLength(
        const std::string& laneName,
        std::size_t currentLength
    );

    double averageWaitTime() const;

    std::int64_t totalThroughput() const;
    std::int64_t totalBlocked() const;
    std::int64_t totalPreemptions() const;

    // Per-intersection totals (TMS-F-042 / TMS-F-052). Every intersection that
    // has recorded any lane activity appears, including zero-throughput ones.
    std::map<std::string, IntersectionTotals> intersectionTotals() const;
    std::int64_t intersectionThroughput(const std::string& intersectionId) const;
    std::int64_t blockedFor(const std::string& intersectionId, const std::string& laneName) const;

    void printEndOfRunReport(int runTicks) const;

private:
    static std::string makeLaneKey(
        const std::string& intersectionId,
        const std::string& laneName
    );
    static std::string intersectionFromKey(const std::string& laneKey);

    std::vector<DequeueEvent> events_;

    std::map<std::string, std::int64_t> blockedEvents_;
    std::map<std::string, std::int64_t> preemptionEvents_;
    std::map<std::string, std::size_t> maxQueueLengths_;
    std::map<std::string, std::int64_t> perLaneThroughput_;
    std::map<std::string, std::int64_t> perLaneWaitTime_;
};

#endif
