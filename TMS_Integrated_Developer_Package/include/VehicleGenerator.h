
#ifndef VEHICLE_GENERATOR_H
#define VEHICLE_GENERATOR_H

#include <cstdint>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>

#include "Vehicle.h"

class VehicleGenerator {
public:
    VehicleGenerator(
        double arrivalRate = 0.4,
        double emergencyProbability = 0.05,
        unsigned int seed = 42
    );

    // Existing methods (backward compatibility)
    void configureLane(
        const std::string& laneName,
        double arrivalRate
    );

    bool shouldGenerate(const std::string& laneName);

    Vehicle generateVehicle(
        const std::string& laneName,
        int currentTick
    );

    // Intersection-aware methods
    void configureLane(
        const std::string& intersectionId,
        const std::string& laneName,
        double arrivalRate
    );

    bool shouldGenerate(
        const std::string& intersectionId,
        const std::string& laneName
    );

    Vehicle generateVehicle(
        const std::string& intersectionId,
        const std::string& laneName,
        int currentTick
    );

private:
    struct LaneRandomState {
        double arrivalRate;
        std::mt19937 rng;

        std::uniform_real_distribution<double> arrivalDist{0.0, 1.0};
        std::uniform_real_distribution<double> emergencyDist{0.0, 1.0};

        LaneRandomState(double rate, unsigned int seed)
            : arrivalRate(rate), rng(seed) {}
    };

    double defaultArrivalRate_;
    double emergencyProbability_;

    std::int64_t nextVehicleId_{1001};

    unsigned int baseSeed_;

    std::unordered_map<std::string, LaneRandomState> laneStates;

    LaneRandomState& getLaneState(
        const std::string& laneKey
    );

    static std::string makeLaneKey(
        const std::string& intersectionId,
        const std::string& laneName
    );

    static void validateProbability(
        double probability,
        const std::string& fieldName
    );

    Vehicle generateVehicleFromState(
        LaneRandomState& state,
        int currentTick
    );
};

#endif
