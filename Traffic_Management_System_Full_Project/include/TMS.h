#ifndef TMS_H
#define TMS_H
#include <algorithm>
#include <cstdint>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace tms {
enum class VehicleType { Normal, Emergency };
enum class SignalPhase { Red, Yellow, Green };
enum class SignalMode { Fixed, Adaptive };
struct Vehicle { std::uint64_t id{}; int arrivalTick{}; VehicleType type{VehicleType::Normal}; std::string originIntersection; };
struct LaneConfig { std::string name; int conflictGroup{}; double arrivalRate{0.2}; };
struct IntersectionConfig { std::string name; std::vector<LaneConfig> lanes; };
struct Config {
  int ticks{1000}; int seed{42}; std::size_t queueCapacity{100};
  int greenDuration{10}, yellowDuration{2}, redDuration{10};
  int adaptiveMaxGreen{30}; double adaptiveFactor{1.0};
  double emergencyProbability{0.02}; int maxPreemptionDelay{3};
  SignalMode signalMode{SignalMode::Fixed}; int renderEvery{1}; bool liveOutput{true};
  std::vector<IntersectionConfig> intersections;
  // link: sourceIntersection.sourceLane -> destinationIntersection.destinationLane
  std::vector<std::pair<std::string,std::string>> links;
  static Config load(const std::string& path);
  void validate() const;
};
class LaneQueue {
 public:
  LaneQueue(std::string name, std::size_t capacity);
  bool enqueue(const Vehicle& v); // emergency vehicles have priority; FIFO within each class
  std::optional<Vehicle> dequeue();
  bool isEmpty() const; bool isFull() const; std::size_t size() const; std::size_t capacity() const;
  const std::string& name() const;
  std::size_t maxObserved() const; bool hasEmergency() const;
 private: std::string name_; std::size_t capacity_, maxObserved_{0}; std::deque<Vehicle> emergency_, normal_;
};
class SignalController {
 public:
  SignalController(int green, int yellow, int red, SignalMode mode=SignalMode::Fixed, int maxGreen=30, double factor=1.0);
  void addLane(const std::string& lane, int conflictGroup);
  void updateSignalState(int tickDelta, const std::map<std::string,std::size_t>& queueLengths={});
  SignalPhase getCurrentPhase(const std::string& laneName) const;
  void preempt(const std::string& laneName);
  bool hasPendingPreemption() const; std::string activeEmergencyLane() const; void clearPreemption();
 private:
  int green_, yellow_, red_, maxGreen_, elapsed_{0}, activeGroup_{0}; double factor_; SignalMode mode_;
  enum class Stage { Green, Yellow, AllRed }; Stage stage_{Stage::Green};
  std::map<std::string,int> laneGroups_; std::vector<int> groups_; std::optional<std::string> preemptLane_; int preemptElapsed_{0};
  int currentGreenDuration_{0};
};
struct LaneStats { long long throughput{0}, totalWait{0}; double averageWait() const { return throughput ? static_cast<double>(totalWait)/throughput : 0.0; } };
struct IntersectionStats { long long throughput{0}, blocked{0}, preemptions{0}; long long totalWait{0}; std::map<std::string,LaneStats> lanes; double averageWait() const { return throughput ? static_cast<double>(totalWait)/throughput : 0.0; } };
class StatisticsCollector {
 public:
  void recordDequeue(const std::string& intersection,const std::string& lane,const Vehicle& v,int tick);
  void recordBlocked(const std::string& intersection,const std::string& lane);
  void recordPreemption(const std::string& intersection);
  void recordQueueLength(const std::string& intersection,const std::string& lane,std::size_t n);
  const std::map<std::string,IntersectionStats>& all() const; long long totalThroughput() const; long long totalBlocked() const;
  void printEndOfRunReport(std::ostream& out) const;
 private: std::map<std::string,IntersectionStats> stats_; std::map<std::string,std::size_t> maxQueues_;
};
class Intersection {
 public:
  explicit Intersection(const IntersectionConfig& cfg,const Config& global);
  const std::string& name() const; std::vector<LaneQueue>& lanes(); const std::vector<LaneQueue>& lanes() const;
  LaneQueue* findLane(const std::string& lane); SignalController& signals(); const SignalController& signals() const;
 private: std::string name_; std::vector<LaneQueue> lanes_; SignalController signals_;
};
class VehicleGenerator {
 public: explicit VehicleGenerator(int seed,double emergencyProbability);
  std::optional<Vehicle> generate(const LaneConfig& lane,int tick,const std::string& intersection,std::uint64_t id);
 private: std::mt19937 rng_; double emergencyProbability_; std::map<std::string,std::poisson_distribution<int>> distributions_;
};
class ConsoleView {
 public: void renderTick(int tick,const std::vector<std::unique_ptr<Intersection>>& intersections,std::ostream& out) const;
};
class SimulationEngine {
 public:
  explicit SimulationEngine(Config config);
  void run(); void requestStop();
  const StatisticsCollector& statistics() const; int currentTick() const;
 private:
  Config config_; std::vector<std::unique_ptr<Intersection>> intersections_; VehicleGenerator generator_; StatisticsCollector stats_; ConsoleView view_;
  std::map<std::string,std::pair<std::string,std::string>> links_; std::uint64_t nextVehicleId_{1}; int tick_{0}; bool stop_{false};
  void step(); void processIntersection(Intersection& intersection);
};
std::string toString(SignalPhase phase);
}
#endif
