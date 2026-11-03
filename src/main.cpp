#include "../include/SimulationEngine.h"
#include <exception>
#include <iostream>
int main() {
 try {
  SimulationEngine simulation;
  Intersection intersection("Main-Intersection");
  intersection.addLane("North_Bound",15); intersection.addLane("South_Bound",15);
  intersection.addLane("East_Bound",15); intersection.addLane("West_Bound",15);
  simulation.addIntersection(intersection);
  simulation.loadConfig(0.4,0.05,false);
  simulation.configureLaneArrivalRate("North_Bound",0.65);
  simulation.configureLaneArrivalRate("South_Bound",0.45);
  simulation.configureLaneArrivalRate("East_Bound",0.30);
  simulation.configureLaneArrivalRate("West_Bound",0.55);
  std::cout << "TRAFFIC MANAGEMENT SYSTEM\\nSingle intersection | Four lanes | 20 simulation ticks\\n\\n";
  simulation.run(20);
 } catch(const std::exception& e) { std::cerr << "Simulation failed: " << e.what() << '\\n'; return 1; }
 return 0;
}