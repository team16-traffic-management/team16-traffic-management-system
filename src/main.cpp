#include "../include/LaneQueue.h"
#include "../include/StatisticsCollector.h"
#include "../include/TrafficSignalController.h"
#include "../vehicle/VehicleGenerator.h"
#include <array>
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <vector>
int main() {
 const std::vector<double> rates={0.65,0.45,0.30,0.55};
 VehicleGenerator generator(rates,2026);
 const std::array<std::string,4> names={{"Lane 1","Lane 2","Lane 3","Lane 4"}};
 std::array<LaneQueue,4> queues={{LaneQueue("Lane 1",20),LaneQueue("Lane 2",20),LaneQueue("Lane 3",20),LaneQueue("Lane 4",20)}};
 SignalController signals(3,1,2);
 for(const auto& name:names) signals.addLane(name);
 StatisticsCollector stats;
 const int ticks=20;
 std::cout<<"TRAFFIC MANAGEMENT SYSTEM\nFour-lane intersection | "<<ticks<<" simulation ticks\n\n";
 for(int tick=1;tick<=ticks;++tick) {
  const auto arrivals=generator.generateForTick(static_cast<std::uint64_t>(tick-1));
  for(const auto& vehicle:arrivals) {
   if(!queues[vehicle.lane].enqueue(vehicle)) stats.recordBlocked("Intersection-1",names[vehicle.lane],tick);
  }
  for(std::size_t lane=0;lane<queues.size();++lane) {
   stats.updateMaxQueueLength("Intersection-1",names[lane],queues[lane].size());
   if(queues[lane].hasEmergency() && signals.preempt(names[lane])) {
    stats.recordPreemption("Intersection-1",names[lane],tick);
    break;
   }
  }
  signals.updateSignalState(1);
  std::size_t greenLane=0;
  for(std::size_t lane=0;lane<queues.size();++lane) {
   if(signals.getCurrentPhase(names[lane])==SignalPhase::GREEN) { greenLane=lane; break; }
  }
  if(!queues[greenLane].isEmpty()) {
   const Vehicle departed=queues[greenLane].dequeue();
   stats.recordDequeue("Intersection-1",names[greenLane],departed,tick);
  }
  std::size_t waiting=0; for(const auto& q:queues) waiting+=q.size();
  std::cout<<"Tick "<<tick<<" | Green: "<<names[greenLane]<<" | Arrivals: "<<arrivals.size()<<" | Waiting: "<<waiting<<'\n';
 }
 std::cout<<"\nSIMULATION SUMMARY\n";
 std::size_t totalWaiting=0;
 for(std::size_t i=0;i<queues.size();++i) {
  std::cout<<names[i]<<" | Still waiting: "<<queues[i].size()<<'\n'; totalWaiting+=queues[i].size();
 }
 std::cout<<"Total vehicles waiting: "<<totalWaiting<<'\n';
 stats.printEndOfRunReport(ticks);
 return 0;
}
