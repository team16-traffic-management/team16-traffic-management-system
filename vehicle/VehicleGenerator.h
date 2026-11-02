#ifndef VEHICLE_GENERATOR_H
#define VEHICLE_GENERATOR_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "Vehicle.h"

class VehicleGenerator {
public:
    // Compatibility with the repository's existing C++14 main.cpp and tests.
    explicit VehicleGenerator(const std::vector<double>& laneArrivalRates,
                              std::uint32_t seed = std::random_device{}());
    VehicleGenerator(std::initializer_list<double> laneArrivalRates,
                     std::uint32_t seed = std::random_device{}());

    // Constructor used by the integrated multi-intersection implementation.
    VehicleGenerator(double arrivalRate = 0.4,
                     double emergencyProbability = 0.05,
                     unsigned int seed = 42);

    void setEmergencyProbability(double probability);
    double getEmergencyProbability() const;

    // Existing per-lane indexed API.
    std::vector<Vehicle> generateForTick(std::uint64_t tick);
    void setArrivalRate(std::size_t lane, double probability);
    void setArrivalRates(const std::vector<double>& probabilities);
    double getArrivalRate(std::size_t lane) const;
    std::size_t getLaneCount() const;

    // Lane-name API used by the integrated implementation.
    void configureLane(const std::string& laneName, double arrivalRate);
    bool shouldGenerate(const std::string& laneName);
    Vehicle generateVehicle(const std::string& laneName, int currentTick);

    // Intersection-aware API used by the integrated implementation.
    void configureLane(const std::string& intersectionId,
                       const std::string& laneName,
                       double arrivalRate);
    bool shouldGenerate(const std::string& intersectionId,
                        const std::string& laneName);
    Vehicle generateVehicle(const std::string& intersectionId,
                            const std::string& laneName,
                            int currentTick);

private:
    struct LaneRandomState {
        double arrivalRate;
        std::mt19937 rng;
        std::uniform_real_distribution<double> arrivalDist;
        std::uniform_real_distribution<double> emergencyDist;

        LaneRandomState(double rate, unsigned int seed)
            : arrivalRate(rate),
              rng(seed),
              arrivalDist(0.0, 1.0),
              emergencyDist(0.0, 1.0) {}
    };

    std::vector<double> arrivalRates_;
    std::mt19937 engine_;
    std::uniform_real_distribution<double> distribution_;
    std::shared_ptr<std::atomic<std::uint64_t> > nextVehicleId_;
    double emergencyProbability_;
    double defaultArrivalRate_;
    unsigned int baseSeed_;
    std::unordered_map<std::string, LaneRandomState> laneStates_;

    static void validateProbability(double probability);
    static std::string makeLaneKey(const std::string& intersectionId,
                                   const std::string& laneName);
    LaneRandomState& getLaneState(const std::string& laneKey);
    std::uint64_t allocateVehicleId();
    Vehicle generateVehicleFromState(LaneRandomState& state,
                                     int currentTick,
                                     const std::string& laneName);
};

#endif
