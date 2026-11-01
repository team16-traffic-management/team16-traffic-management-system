#include "TMS.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

namespace tms {
static std::string trim(std::string s){ auto a=s.find_first_not_of(" \t\r\n"); if(a==std::string::npos)return {}; auto b=s.find_last_not_of(" \t\r\n"); return s.substr(a,b-a+1); }
Config Config::load(const std::string& path){
 Config c; c.intersections.clear(); std::ifstream f(path); if(!f) throw std::runtime_error("Cannot open configuration file: "+path);
 std::string line; std::map<std::string,IntersectionConfig> ix;
 while(std::getline(f,line)){ line=trim(line); if(line.empty()||line[0]=='#')continue; auto p=line.find('='); if(p==std::string::npos)throw std::runtime_error("Malformed config line: "+line); auto k=trim(line.substr(0,p)),v=trim(line.substr(p+1));
  try {
   if(k=="ticks")c.ticks=std::stoi(v); else if(k=="seed")c.seed=std::stoi(v); else if(k=="queue_capacity")c.queueCapacity=static_cast<std::size_t>(std::stoul(v));
   else if(k=="green_duration")c.greenDuration=std::stoi(v); else if(k=="yellow_duration")c.yellowDuration=std::stoi(v); else if(k=="red_duration")c.redDuration=std::stoi(v);
   else if(k=="adaptive_max_green")c.adaptiveMaxGreen=std::stoi(v); else if(k=="adaptive_factor")c.adaptiveFactor=std::stod(v); else if(k=="emergency_probability")c.emergencyProbability=std::stod(v);
   else if(k=="max_preemption_delay")c.maxPreemptionDelay=std::stoi(v); else if(k=="render_every")c.renderEvery=std::stoi(v); else if(k=="live_output")c.liveOutput=(v=="true"||v=="1");
   else if(k=="signal_mode"){if(v=="fixed")c.signalMode=SignalMode::Fixed;else if(v=="adaptive")c.signalMode=SignalMode::Adaptive;else throw std::runtime_error("signal_mode must be fixed or adaptive");}
   else if(k.rfind("intersection.",0)==0){ // intersection.NAME.lane=LANE,GROUP,RATE
    auto rest=k.substr(13); auto dot=rest.find(".lane"); if(dot==std::string::npos)throw std::runtime_error("Expected intersection.NAME.lane"); auto name=rest.substr(0,dot); std::stringstream ss(v); std::string lane,group,rate; if(!std::getline(ss,lane,',')||!std::getline(ss,group,',')||!std::getline(ss,rate,','))throw std::runtime_error("Lane format: lane,group,arrival_rate");
    ix[name].name=name; ix[name].lanes.push_back({trim(lane),std::stoi(group),std::stod(rate)});
   } else if(k=="link"){auto q=v.find("->");if(q==std::string::npos)throw std::runtime_error("Link format: SOURCE. LANE -> DEST.LANE");c.links.push_back({trim(v.substr(0,q)),trim(v.substr(q+2))});}
   else throw std::runtime_error("Unknown configuration key: "+k);
  } catch(const std::invalid_argument&){throw std::runtime_error("Invalid value for config key: "+k);} catch(const std::out_of_range&){throw std::runtime_error("Out-of-range value for config key: "+k);}
 }
 for(auto& kv:ix)c.intersections.push_back(kv.second); c.validate(); return c;
}
void Config::validate() const {
 if(ticks<=0||ticks>10000000)throw std::runtime_error("ticks must be 1..10000000"); if(queueCapacity==0||queueCapacity>1000000)throw std::runtime_error("queue_capacity must be 1..1000000");
 if(greenDuration<=0||yellowDuration<=0||redDuration<0||adaptiveMaxGreen<greenDuration||adaptiveFactor<0)throw std::runtime_error("Invalid signal durations/adaptive settings");
 if(emergencyProbability<0||emergencyProbability>1||maxPreemptionDelay<0||renderEvery<=0)throw std::runtime_error("Invalid probability, preemption delay, or render interval");
 if(intersections.empty())throw std::runtime_error("At least one intersection and one lane are required"); std::set<std::string> names;
 for(const auto& i:intersections){if(i.name.empty()||i.lanes.empty()||!names.insert(i.name).second)throw std::runtime_error("Intersection names must be unique and contain lanes");std::set<std::string> lanes;for(const auto& l:i.lanes)if(l.name.empty()||l.conflictGroup<0||l.arrivalRate<0||l.arrivalRate>100||!lanes.insert(l.name).second)throw std::runtime_error("Invalid or duplicate lane in "+i.name);}
}
LaneQueue::LaneQueue(std::string n,std::size_t c):name_(std::move(n)),capacity_(c){if(name_.empty()||!c)throw std::invalid_argument("Lane name/capacity invalid");}
bool LaneQueue::enqueue(const Vehicle& v){if(isFull())return false;(v.type==VehicleType::Emergency?emergency_:normal_).push_back(v);maxObserved_=std::max(maxObserved_,size());return true;}
std::optional<Vehicle> LaneQueue::dequeue(){if(!emergency_.empty()){auto v=emergency_.front();emergency_.pop_front();return v;}if(!normal_.empty()){auto v=normal_.front();normal_.pop_front();return v;}return std::nullopt;}
bool LaneQueue::isEmpty()const{return emergency_.empty()&&normal_.empty();} bool LaneQueue::isFull()const{return size()>=capacity_;} std::size_t LaneQueue::size()const{return emergency_.size()+normal_.size();} std::size_t LaneQueue::capacity()const{return capacity_;} const std::string& LaneQueue::name()const{return name_;} std::size_t LaneQueue::maxObserved()const{return maxObserved_;} bool LaneQueue::hasEmergency()const{return !emergency_.empty();}
SignalController::SignalController(int g,int y,int r,SignalMode m,int mg,double f):green_(g),yellow_(y),red_(r),maxGreen_(mg),factor_(f),mode_(m),currentGreenDuration_(g){if(g<=0||y<=0||r<0||mg<g||f<0)throw std::invalid_argument("Invalid signal timing");}
void SignalController::addLane(const std::string& n,int group){if(group<0)throw std::invalid_argument("Negative conflict group");laneGroups_[n]=group;if(std::find(groups_.begin(),groups_.end(),group)==groups_.end()){groups_.push_back(group);std::sort(groups_.begin(),groups_.end());}}
void SignalController::updateSignalState(int delta,const std::map<std::string,std::size_t>& queues){if(delta<0)throw std::invalid_argument("tick delta cannot be negative"); if(preemptLane_){preemptElapsed_+=delta;return;} elapsed_+=delta;
 if(stage_==Stage::Green){int duration=green_;if(mode_==SignalMode::Adaptive){std::size_t q=0;for(auto& x:laneGroups_)if(x.second==activeGroup_){auto it=queues.find(x.first);if(it!=queues.end())q+=it->second;}duration=std::min(maxGreen_,green_+static_cast<int>(std::ceil(q*factor_)));}currentGreenDuration_=duration;if(elapsed_>=duration){elapsed_=0;stage_=Stage::Yellow;}}
 else if(stage_==Stage::Yellow&&elapsed_>=yellow_){elapsed_=0;stage_=Stage::AllRed;}
 else if(stage_==Stage::AllRed&&elapsed_>=red_){elapsed_=0;stage_=Stage::Green;if(!groups_.empty()){auto it=std::find(groups_.begin(),groups_.end(),activeGroup_);activeGroup_=(it==groups_.end()||++it==groups_.end())?groups_.front():*it;}}
}
SignalPhase SignalController::getCurrentPhase(const std::string& lane)const{auto it=laneGroups_.find(lane);if(it==laneGroups_.end())return SignalPhase::Red;if(preemptLane_)return lane==*preemptLane_?SignalPhase::Green:SignalPhase::Red;if(it->second!=activeGroup_)return SignalPhase::Red;if(stage_==Stage::Green)return SignalPhase::Green;if(stage_==Stage::Yellow)return SignalPhase::Yellow;return SignalPhase::Red;}
void SignalController::preempt(const std::string& lane){if(!laneGroups_.count(lane))throw std::invalid_argument("Unknown lane for preemption: "+lane);preemptLane_=lane;preemptElapsed_=0;}
bool SignalController::hasPendingPreemption()const{return preemptLane_.has_value();}std::string SignalController::activeEmergencyLane()const{return preemptLane_.value_or("");} void SignalController::clearPreemption(){preemptLane_.reset();preemptElapsed_=0;elapsed_=0;stage_=Stage::AllRed;}
void StatisticsCollector::recordDequeue(const std::string& ix,const std::string& lane,const Vehicle& v,int tick){auto& s=stats_[ix];++s.throughput;long long w=std::max(0,tick-v.arrivalTick);s.totalWait+=w;auto& l=s.lanes[lane];++l.throughput;l.totalWait+=w;}
void StatisticsCollector::recordBlocked(const std::string& ix,const std::string&){++stats_[ix].blocked;}void StatisticsCollector::recordPreemption(const std::string& ix){++stats_[ix].preemptions;}
void StatisticsCollector::recordQueueLength(const std::string& ix,const std::string& lane,std::size_t n){auto key=ix+"."+lane;maxQueues_[key]=std::max(maxQueues_[key],n);}
const std::map<std::string,IntersectionStats>& StatisticsCollector::all()const{return stats_;}long long StatisticsCollector::totalThroughput()const{long long n=0;for(auto&x:stats_)n+=x.second.throughput;return n;}long long StatisticsCollector::totalBlocked()const{long long n=0;for(auto&x:stats_)n+=x.second.blocked;return n;}
void StatisticsCollector::printEndOfRunReport(std::ostream& out)const{long long count=0,wait=0,blocked=0;out<<"\n========== TRAFFIC MANAGEMENT SYSTEM REPORT ==========\n";for(auto&x:stats_){auto&s=x.second;count+=s.throughput;wait+=s.totalWait;blocked+=s.blocked;out<<"Intersection: "<<x.first<<" | Throughput: "<<s.throughput<<" | Average wait: "<<s.averageWait()<<" ticks | Blocked: "<<s.blocked<<" | Preemptions: "<<s.preemptions<<"\n";for(auto&l:s.lanes){auto key=x.first+"."+l.first;out<<"  Lane "<<l.first<<": throughput="<<l.second.throughput<<", average_wait="<<l.second.averageWait()<<", max_queue="<<(maxQueues_.count(key)?maxQueues_.at(key):0)<<"\n";}}out<<"SYSTEM TOTALS | Throughput: "<<count<<" | Average wait: "<<(count?static_cast<double>(wait)/count:0.0)<<" ticks | Blocked: "<<blocked<<"\n=======================================================\n";}
Intersection::Intersection(const IntersectionConfig& c,const Config& g):name_(c.name),signals_(g.greenDuration,g.yellowDuration,g.redDuration,g.signalMode,g.adaptiveMaxGreen,g.adaptiveFactor){for(auto&l:c.lanes){lanes_.emplace_back(l.name,g.queueCapacity);signals_.addLane(l.name,l.conflictGroup);}}
const std::string& Intersection::name()const{return name_;}std::vector<LaneQueue>& Intersection::lanes(){return lanes_;}const std::vector<LaneQueue>& Intersection::lanes()const{return lanes_;}LaneQueue* Intersection::findLane(const std::string& n){for(auto&l:lanes_)if(l.name()==n)return &l;return nullptr;}SignalController& Intersection::signals(){return signals_;}const SignalController& Intersection::signals()const{return signals_;}
VehicleGenerator::VehicleGenerator(int seed,double p):rng_(seed),emergencyProbability_(p){}
std::optional<Vehicle> VehicleGenerator::generate(const LaneConfig& l,int tick,const std::string& ix,std::uint64_t id){auto it=distributions_.find(ix+"."+l.name);if(it==distributions_.end())it=distributions_.emplace(ix+"."+l.name,std::poisson_distribution<int>(l.arrivalRate)).first;if(it->second(rng_)<=0)return std::nullopt;std::bernoulli_distribution emergency(emergencyProbability_);return Vehicle{id,tick,emergency(rng_)?VehicleType::Emergency:VehicleType::Normal,ix};}
std::string toString(SignalPhase p){switch(p){case SignalPhase::Green:return "GREEN";case SignalPhase::Yellow:return "YELLOW";default:return "RED";}}
void ConsoleView::renderTick(int tick,const std::vector<std::unique_ptr<Intersection>>& all,std::ostream& out)const{out<<"[Tick "<<tick<<"]\n";for(auto&ip:all){out<<"  "<<ip->name()<<": ";for(auto&l:ip->lanes())out<<l.name()<<"="<<toString(ip->signals().getCurrentPhase(l.name()))<<"/q"<<l.size()<<" ";out<<"\n";}}
SimulationEngine::SimulationEngine(Config c):config_(std::move(c)),generator_(config_.seed,config_.emergencyProbability){config_.validate();for(auto&i:config_.intersections)intersections_.push_back(std::make_unique<Intersection>(i,config_));for(auto&link:config_.links){auto parse=[](const std::string&s){auto p=s.find('.');if(p==std::string::npos)throw std::runtime_error("Link endpoint must be Intersection.Lane: "+s);return std::pair<std::string,std::string>{s.substr(0,p),s.substr(p+1)};};auto a=parse(link.first),b=parse(link.second);bool source=false,dest=false;for(auto&ip:intersections_){if(ip->name()==a.first&&ip->findLane(a.second))source=true;if(ip->name()==b.first&&ip->findLane(b.second))dest=true;}if(!source||!dest)throw std::runtime_error("Link references unknown intersection/lane");links_[a.first+"."+a.second]=b;}}
void SimulationEngine::requestStop(){stop_=true;}int SimulationEngine::currentTick()const{return tick_;}const StatisticsCollector& SimulationEngine::statistics()const{return stats_;}
void SimulationEngine::processIntersection(Intersection& ix){
 std::map<std::string,std::size_t> sizes; for(auto&l:ix.lanes()) sizes[l.name()]=l.size();
 // A waiting emergency vehicle requests an immediate, logged signal preemption.
 if(!ix.signals().hasPendingPreemption()) for(auto&l:ix.lanes()) if(l.hasEmergency()) { ix.signals().preempt(l.name()); stats_.recordPreemption(ix.name()); break; }
 ix.signals().updateSignalState(1,sizes);
 for(auto&l:ix.lanes()){
  if(ix.signals().getCurrentPhase(l.name())==SignalPhase::Green&&!l.isEmpty()){
   auto v=l.dequeue();
   if(v){ stats_.recordDequeue(ix.name(),l.name(),*v,tick_);
    // Linked vehicles are queued downstream after all intersections have processed this tick.
    auto key=ix.name()+"."+l.name(); auto link=links_.find(key);
    if(link!=links_.end()){ for(auto&dest:intersections_) if(dest->name()==link->second.first){ auto*q=dest->findLane(link->second.second); if(q){ Vehicle moved=*v; moved.arrivalTick=tick_+1; moved.originIntersection=ix.name(); if(!q->enqueue(moved))stats_.recordBlocked(dest->name(),q->name()); } } }
    if(v->type==VehicleType::Emergency) ix.signals().clearPreemption();
   }
  }
  stats_.recordQueueLength(ix.name(),l.name(),l.size());
 }
}
void SimulationEngine::step(){for(std::size_t i=0;i<intersections_.size();++i){auto&ix=*intersections_[i];const auto&ic=config_.intersections[i];for(auto&lc:ic.lanes){auto v=generator_.generate(lc,tick_,ix.name(),nextVehicleId_);if(v){++nextVehicleId_;if(!ix.findLane(lc.name)->enqueue(*v))stats_.recordBlocked(ix.name(),lc.name);}}
  processIntersection(ix);
 }
 if(config_.liveOutput&&tick_%config_.renderEvery==0)view_.renderTick(tick_,intersections_,std::cout);
}
void SimulationEngine::run(){try{for(tick_=0;tick_<config_.ticks&&!stop_;++tick_)step();}catch(const std::exception&e){std::cerr<<"[ERROR] Simulation stopped safely at tick "<<tick_<<": "<<e.what()<<"\n";}stats_.printEndOfRunReport(std::cout);}
}
