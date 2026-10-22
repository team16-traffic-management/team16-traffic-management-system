
#include "StatisticsCollector.h"
#include "ConsoleView.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

int main() {
    std::cout << "Running StatisticsCollector tests...\n";

    StatisticsCollector stats;

    // Test 1: Empty statistics.
    assert(stats.totalThroughput() == 0);
    assert(stats.averageWaitTime() == 0.0);
    assert(stats.maximumQueueLengthForLane("Lane A") == 0);

    std::cout << "PASS: Empty statistics\n";

    // Test 2: Record completed vehicles.
    Vehicle v1{1, 0, Vehicle::Type::NORMAL};
    Vehicle v2{2, 2, Vehicle::Type::NORMAL};
    Vehicle v3{3, 3, Vehicle::Type::EMERGENCY};

    stats.recordDequeue("Lane A", v1, 5);  // Wait = 5
    stats.recordDequeue("Lane A", v2, 6);  // Wait = 4
    stats.recordDequeue("Lane B", v3, 8);  // Wait = 5

    assert(stats.totalThroughput() == 3);
    assert(std::abs(stats.averageWaitTime() - (14.0 / 3.0)) < 0.001);
    assert(std::abs(stats.averageWaitTimeForLane("Lane A") - 4.5) < 0.001);
    assert(std::abs(stats.averageWaitTimeForLane("Lane B") - 5.0) < 0.001);

    std::cout << "PASS: Throughput and waiting-time calculations\n";

    // Test 3: Reject a vehicle leaving before its arrival.
    Vehicle invalidVehicle{4, 10, Vehicle::Type::NORMAL};
    stats.recordDequeue("Lane A", invalidVehicle, 7);

    assert(stats.totalThroughput() == 3);

    std::cout << "PASS: Invalid dequeue rejected\n";

    // Test 4: Track maximum queue lengths.
    stats.recordQueueLength("Lane A", 2);
    stats.recordQueueLength("Lane A", 5);
    stats.recordQueueLength("Lane A", 3);
    stats.recordQueueLength("Lane B", 4);
    stats.recordQueueLength("Lane B", -1);

    assert(stats.maximumQueueLengthForLane("Lane A") == 5);
    assert(stats.maximumQueueLengthForLane("Lane B") == 4);

    std::cout << "PASS: Maximum queue length tracking\n";

    // Test 5: Verify console output using the existing interfaces.
    LaneQueue laneA("Lane A", 10);
    LaneQueue laneB("Lane B", 10);

    laneA.mockPush(v1);
    laneA.mockPush(v2);
    laneB.mockPush(v3);

    std::vector<LaneQueue> lanes{laneA, laneB};

    SignalController signals;
    signals.mockSetColor("Lane A", SignalColor::GREEN);
    signals.mockSetColor("Lane B", SignalColor::RED);

    ConsoleView view;
    view.renderTick(5, lanes, signals);

    std::cout << "PASS: Console rendering executed\n";

    // Test 6: Print the final statistics report.
    stats.printEndOfRunReport();

    std::cout << "\nAll statistics and console tests passed.\n";

    return 0;
}
