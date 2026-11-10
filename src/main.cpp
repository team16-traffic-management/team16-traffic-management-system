#include "../include/SimulationEngine.h"
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    installSimulationSignalHandlers();
    try {
        SimulationEngine simulation;
        int ticks = 20;
        if (argc > 1) {
            ticks = simulation.loadConfigFromFile(argv[1]);
        } else {
            Intersection intersection("Main-Intersection");
            intersection.addLane("North_Bound", 15);
            intersection.addLane("South_Bound", 15);
            intersection.addLane("East_Bound", 15);
            intersection.addLane("West_Bound", 15);
            simulation.addIntersection(intersection);
            simulation.loadConfig(0.4, 0.05, false);
            simulation.configureLaneArrivalRate("North_Bound", 0.65);
            simulation.configureLaneArrivalRate("South_Bound", 0.45);
            simulation.configureLaneArrivalRate("East_Bound", 0.30);
            simulation.configureLaneArrivalRate("West_Bound", 0.55);
        }
        std::cout << "TRAFFIC MANAGEMENT SYSTEM\n"
                  << simulation.getIntersectionCount()
                  << " intersection(s) | ";
        if (ticks == 0) std::cout << "run until stopped\n";
        else std::cout << ticks << " simulation ticks\n";
        std::cout << "Press Ctrl+C at any time to stop safely and print the summary.\n";
        simulation.run(ticks);
    } catch (const std::exception& error) {
        std::cerr << "Simulation failed: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
