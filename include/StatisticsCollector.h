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

    void printEndOfRunReport(int runTicks) const;

private:
    static std::string makeLaneKey(
        const std::string& intersectionId,
        const std::string& laneName
    );

    std::vector<DequeueEvent> events_;

    std::map<std::string, std::int64_t> blockedEvents_;
    std::map<std::string, std::int64_t> preemptionEvents_;
    std::map<std::string, std::size_t> maxQueueLengths_;
    std::map<std::string, std::int64_t> perLaneThroughput_;
    std::map<std::string, std::int64_t> perLaneWaitTime_;
};

#endif
