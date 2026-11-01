
#pragma once

#include <map>
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
    // Record a vehicle leaving a lane.
    void recordDequeue(
        const std::string& laneName,
        const Vehicle& vehicle,
        int dequeueTick
    );

    // Record the queue length observed during simulation.
    void recordQueueLength(
        const std::string& laneName,
        int queueLength
    );

    // Return the average waiting time across all completed vehicles.
    double averageWaitTime() const;

    // Return the average waiting time for one lane.
    double averageWaitTimeForLane(
        const std::string& laneName
    ) const;

    // Return the number of vehicles that have left their lanes.
    int totalThroughput() const;

    // Return the maximum observed queue length for one lane.
    int maximumQueueLengthForLane(
        const std::string& laneName
    ) const;

    // Print the final simulation statistics.
    void printEndOfRunReport() const;

private:
    std::vector<DequeueEvent> events_;
    std::map<std::string, int> maximumQueueLengths_;
};
