
#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <cstddef>
#include <deque>

#include "Intersection.h"
#include "VehicleGenerator.h"
#include "StatisticsCollector.h"
#include "ConsoleView.h"

class SimulationEngine {
public:
    SimulationEngine();

    void loadConfig(
        double arrivalRate = 0.4,
        double emergencyProb = 0.05,
        bool adaptiveMode = false
    );

    void addIntersection(const Intersection& intersection);

    void configureLaneArrivalRate(
        const std::string& laneName,
        double arrivalRate
    );

    // Connect two intersections using matching lane names.
    void connectIntersections(
        const std::string& sourceIntersectionId,
        const std::string& destinationIntersectionId
    );

    int loadConfigFromFile(const std::string& filename);

    int getConfiguredQueueCapacity() const;

    int configuredQueueCapacity_{15};

    void run(int maxTicks, bool silent = false);

    void tick(bool silent = false);

    const StatisticsCollector& getStats() const;

    // Returns the number of successfully transferred vehicles.
    std::size_t getTotalTransfers() const;

private:
    void deliverPendingTransfers();
    // Defines a connection between two intersections.
    struct IntersectionConnection {
        std::string sourceIntersectionId;
        std::string destinationIntersectionId;
        std::string laneName;
    };

    // Stores vehicles waiting to enter their destination intersection.
    struct PendingTransfer {
        std::string destinationIntersectionId;
        std::string laneName;
        Vehicle vehicle;
    };

    std::vector<Intersection> intersections_;

    // Registered intersection-to-intersection connections.
    std::vector<IntersectionConnection> connections_;

    // Vehicles waiting for transfer to the next intersection.
    std::deque<PendingTransfer> pendingTransfers_;

    VehicleGenerator generator_;

    StatisticsCollector stats_;

    ConsoleView view_;

    int currentTick_{0};

    // Total number of successful vehicle transfers.
    std::size_t totalTransfers_{0};
};

#endif // SIMULATION_ENGINE_H
