// StatisticsCollector.h
// Records dequeue events and computes wait-time and throughput stats.
// Satisfies TMS-F-013 (wait time), TMS-F-050 (avg wait), TMS-F-052 (throughput).
#pragma once

#include <string>
#include <vector>
#include "Vehicle.h"

struct DequeueEvent {
    std::string laneName;
    Vehicle vehicle;
    int dequeueTick;
};

class StatisticsCollector {
public:
    // Call this every time a vehicle leaves a lane (dequeued on Green).
    void recordDequeue(const std::string& laneName, const Vehicle& v, int dequeueTick);

    double averageWaitTime() const;
    double averageWaitTimeForLane(const std::string& laneName) const;
    int totalThroughput() const;

    // Prints the final summary report (TMS-F-050/051/052).
    void printEndOfRunReport() const;

private:
    std::vector<DequeueEvent> events_;
};
