#ifndef STATISTICS_COLLECTOR_H
#define STATISTICS_COLLECTOR_H
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "../vehicle/Vehicle.h"
struct DequeueEvent { std::string intersectionId; std::string laneName; int vehicleId; int waitTime; int dequeueTick; bool isEmergency; };
struct CountEvent { std::string intersectionId; std::string laneName; int tick; };
class StatisticsCollector {
public:
 void recordDequeue(const std::string&,const std::string&,const Vehicle&,int);
 void recordBlocked(const std::string&,const std::string&,int);
 void recordPreemption(const std::string&,const std::string&,int);
 void updateMaxQueueLength(const std::string&,const std::string&,std::size_t);
 void recordDequeue(const std::string&,const Vehicle&,int);
 void recordBlocked(const std::string&,int);
 void recordPreemption(const std::string&,int);
 void updateMaxQueueLength(const std::string&,std::size_t);
 double averageWaitTime() const;
 std::int64_t totalThroughput() const;
 std::int64_t totalBlocked() const;
 std::int64_t totalPreemptions() const;
 const std::vector<DequeueEvent>& getDequeueEvents() const;
 const std::vector<CountEvent>& getBlockedEvents() const;
 const std::vector<CountEvent>& getPreemptionEvents() const;
 void printEndOfRunReport(int runTicks) const;
private:
 static std::string makeLaneKey(const std::string&,const std::string&);
 std::vector<DequeueEvent> events_;
 std::vector<CountEvent> blockedTimestamps_,preemptionTimestamps_;
 std::map<std::string,std::size_t> maxQueueLengths_;
 std::map<std::string,std::int64_t> perLaneThroughput_,perLaneWaitTime_;
};
#endif
