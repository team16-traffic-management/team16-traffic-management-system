#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H
#include <cstddef>
#include <deque>
#include <string>
#include <vector>
#include "Intersection.h"
#include "StatisticsCollector.h"
#include "../vehicle/VehicleGenerator.h"
#include "ConsoleView.h"

class SimulationEngine {
public:
    SimulationEngine();
    void loadConfig(double arrivalRate = 0.4, double emergencyProb = 0.05, bool adaptiveMode = false);
    void addIntersection(const Intersection& intersection);
    void configureLaneArrivalRate(const std::string& laneName, double arrivalRate);
    void connectIntersections(const std::string& sourceIntersectionId, const std::string& destinationIntersectionId);
    int loadConfigFromFile(const std::string& filename);
    int getConfiguredQueueCapacity() const;
    void run(int maxTicks, bool silent = false);
    void tick(bool silent = false);
    const StatisticsCollector& getStats() const;
    std::size_t getTotalTransfers() const;
private:
    struct IntersectionConnection { std::string sourceIntersectionId; std::string destinationIntersectionId; std::string laneName; };
    struct PendingTransfer { std::string destinationIntersectionId; std::string laneName; Vehicle vehicle; };
    void deliverPendingTransfers();
    int configuredQueueCapacity_{15};
    std::vector<Intersection> intersections_;
    std::vector<IntersectionConnection> connections_;
    std::deque<PendingTransfer> pendingTransfers_;
    VehicleGenerator generator_;
    StatisticsCollector stats_;
    ConsoleView view_;
    int currentTick_{0};
    std::size_t totalTransfers_{0};
};
#endif // SIMULATION_ENGINE_H
