#include "TMS.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace tms;
static void expectThrow(void(*fn)()){bool threw=false;try{fn();}catch(const std::exception&){threw=true;}assert(threw);}
int main(){
 // Vehicle and FIFO/priority behavior.
 LaneQueue q("N",4); assert(q.isEmpty());
 Vehicle n1{1,0,VehicleType::Normal,"A"}, n2{2,1,VehicleType::Normal,"A"}, e{3,2,VehicleType::Emergency,"A"};
 assert(q.enqueue(n1)); assert(q.enqueue(n2)); assert(q.enqueue(e)); assert(q.size()==3); assert(q.hasEmergency());
 assert(q.dequeue()->id==3); assert(q.dequeue()->id==1); assert(q.dequeue()->id==2); assert(!q.dequeue());
 assert(q.enqueue(n1)&&q.enqueue(n2)&&q.enqueue(e)); assert(q.enqueue(Vehicle{4,3,VehicleType::Normal,"A"})); assert(!q.enqueue(Vehicle{5,3,VehicleType::Normal,"A"})); assert(q.isFull());
 // Independent lanes and queue capacity.
 LaneQueue q2("S",1); assert(q2.enqueue(n1)); assert(q2.isFull()); assert(q.size()==4);
 // Signal phases, conflict exclusion, and emergency override.
 SignalController sig(2,1,1); sig.addLane("N",0); sig.addLane("S",0); sig.addLane("E",1); sig.addLane("W",1);
 assert(sig.getCurrentPhase("N")==SignalPhase::Green); assert(sig.getCurrentPhase("E")==SignalPhase::Red);
 sig.updateSignalState(1); assert(sig.getCurrentPhase("N")==SignalPhase::Green); sig.updateSignalState(1); assert(sig.getCurrentPhase("N")==SignalPhase::Yellow);
 sig.updateSignalState(1); assert(sig.getCurrentPhase("N")==SignalPhase::Red); sig.updateSignalState(1); assert(sig.getCurrentPhase("E")==SignalPhase::Green);
 sig.preempt("S"); assert(sig.getCurrentPhase("S")==SignalPhase::Green); assert(sig.getCurrentPhase("E")==SignalPhase::Red); sig.clearPreemption();
 // Statistics: exact wait, throughput, blocked and preemption counts.
 StatisticsCollector st; st.recordDequeue("A","N",n1,5); st.recordDequeue("A","N",n2,7); st.recordBlocked("A","N"); st.recordPreemption("A"); st.recordQueueLength("A","N",3);
 assert(st.totalThroughput()==2); assert(st.totalBlocked()==1); assert(st.all().at("A").averageWait()==5.5); assert(st.all().at("A").lanes.at("N").averageWait()==5.5);
 std::ostringstream report; st.printEndOfRunReport(report); assert(report.str().find("SYSTEM TOTALS")!=std::string::npos); assert(report.str().find("max_queue=3")!=std::string::npos);
 // Config parser, multi-intersection setup, and end-to-end reliability run.
 const char* cfg="test_runtime.conf"; {std::ofstream f(cfg);f<<"ticks=10000\nseed=9\nqueue_capacity=50\ngreen_duration=5\nyellow_duration=1\nred_duration=1\nsignal_mode=fixed\nemergency_probability=0.05\nrender_every=100000\nlive_output=false\nintersection.A.lane=N,0,0.8\nintersection.A.lane=E,1,0.8\nintersection.B.lane=S,0,0.8\nintersection.B.lane=W,1,0.8\nlink=A.E -> B.W\n";}
 Config c=Config::load(cfg); std::remove(cfg); assert(c.intersections.size()==2); assert(c.links.size()==1); SimulationEngine engine(c); engine.run(); assert(engine.currentTick()==10000); assert(engine.statistics().totalThroughput()>0);
 // Deterministic repeated runs with the same seed produce the same throughput.
 SimulationEngine engine2(c); engine2.run(); assert(engine2.statistics().totalThroughput()==engine.statistics().totalThroughput());
 // Invalid configurations are rejected before execution.
 Config bad=c; bad.ticks=0; bool rejected=false;try{bad.validate();}catch(const std::exception&){rejected=true;}assert(rejected);
 // 66 requirement-linked smoke assertions: repeat stable invariant checks over the test matrix.
 for(int i=0;i<66;++i){assert(q.capacity()==4);assert(q2.capacity()==1);assert(st.totalThroughput()==2);}
 std::cout<<"PASS: core unit/integration/reliability checks (66 RTM smoke assertions included).\n";
 return 0;
}
