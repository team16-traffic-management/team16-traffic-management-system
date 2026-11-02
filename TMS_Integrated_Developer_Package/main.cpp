
#include <iostream>
#include <chrono>
#include <exception>

#include "SimulationEngine.h"
#include "Intersection.h"

int main() {
    try {
        std::cout << "==================================================\n";
        std::cout << " STARTING TRAFFIC MANAGEMENT SIMULATION\n";
        std::cout << "==================================================\n";

        SimulationEngine engine;

        int totalTicks = engine.loadConfigFromFile("config.txt");
        int queueCapacity = engine.getConfiguredQueueCapacity();

        // INTERSECTION 1
        Intersection intersection1("INT_01");

        intersection1.addLane("North_Bound", queueCapacity);
        intersection1.addLane("South_Bound", queueCapacity);
        intersection1.addLane("East_Bound", queueCapacity);
        intersection1.addLane("West_Bound", queueCapacity);

        engine.addIntersection(intersection1);

        // INTERSECTION 2
        Intersection intersection2("INT_02");

        intersection2.addLane("North_Bound", queueCapacity);
        intersection2.addLane("South_Bound", queueCapacity);
        intersection2.addLane("East_Bound", queueCapacity);
        intersection2.addLane("West_Bound", queueCapacity);

        engine.addIntersection(intersection2);

        // CONNECT INTERSECTIONS ONLY AFTER BOTH ARE REGISTERED
        engine.connectIntersections("INT_01", "INT_02");

        // Reload configuration to apply settings to both intersections.
        totalTicks = engine.loadConfigFromFile("config.txt");

        std::cout << "\nConfiguration:\n";
        std::cout << "Total ticks: " << totalTicks << "\n";
        std::cout << "Queue capacity: " << queueCapacity << "\n";
        std::cout << "Number of intersections: 2\n";
        std::cout << "Intersection IDs: INT_01, INT_02\n";
        std::cout << "Lanes per intersection: 4\n";
        std::cout << "Arrival and emergency settings: loaded from config.txt\n";
        std::cout << "Simulation output: Silent performance mode\n\n";

        auto startTime = std::chrono::steady_clock::now();

        engine.run(totalTicks, true);

        auto endTime = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> duration =
            endTime - startTime;

        double ticksPerSec =
            totalTicks / (duration.count() / 1000.0);

        std::cout << "\nExecution Summary:\n";
        std::cout << "Total Execution Time: "
                  << duration.count() << " ms\n";
        std::cout << "Processing Throughput: "
                  << ticksPerSec << " ticks/sec\n";
    }
    catch (const std::exception& error) {
        std::cerr << "\nConfiguration/Simulation Error: "
                  << error.what() << "\n";
        return 1;
    }

    return 0;
}
