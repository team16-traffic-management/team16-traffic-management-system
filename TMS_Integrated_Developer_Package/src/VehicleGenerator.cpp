
#include "VehicleGenerator.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

VehicleGenerator::VehicleGenerator(
    double arrivalRate,
    double emergencyProbability,
    unsigned int seed
)
    : defaultArrivalRate_(arrivalRate),
      emergencyProbability_(emergencyProbability),
      baseSeed_(seed) {

    validateProbability(arrivalRate, "Arrival rate");
    validateProbability(
        emergencyProbability,
        "Emergency probability"
    );
}

void VehicleGenerator::validateProbability(
    double probability,
    const std::string& fieldName
) {
    if (!std::isfinite(probability) ||
        probability < 0.0 ||
        probability > 1.0) {

        throw std::invalid_argument(
            "Invalid " + fieldName +
            ": value must be between 0 and 1"
        );
    }
}

std::string VehicleGenerator::makeLaneKey(
    const std::string& intersectionId,
    const std::string& laneName
) {
    if (intersectionId.empty() || laneName.empty()) {
        throw std::invalid_argument(
            "Intersection ID and lane name cannot be empty"
        );
    }

    return intersectionId + "::" + laneName;
}

VehicleGenerator::LaneRandomState&
VehicleGenerator::getLaneState(
    const std::string& laneKey
) {
    if (laneKey.empty()) {
        throw std::invalid_argument(
            "Lane key cannot be empty"
        );
    }

    auto it = laneStates.find(laneKey);

    if (it == laneStates.end()) {
        unsigned int laneSeed = baseSeed_;

        // Create a deterministic but independent seed for each lane.
        for (unsigned char ch : laneKey) {
            laneSeed = laneSeed * 31u + ch;
        }

        it = laneStates.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(laneKey),
            std::forward_as_tuple(
                defaultArrivalRate_,
                laneSeed
            )
        ).first;
    }

    return it->second;
}

// --------------------------------------------------
// BACKWARD-COMPATIBLE METHODS
// --------------------------------------------------

void VehicleGenerator::configureLane(
    const std::string& laneName,
    double arrivalRate
) {
    validateProbability(arrivalRate, "Lane arrival rate");

    if (laneName.empty()) {
        throw std::invalid_argument(
            "Lane name cannot be empty"
        );
    }

    getLaneState(laneName).arrivalRate = arrivalRate;
}

bool VehicleGenerator::shouldGenerate(
    const std::string& laneName
) {
    LaneRandomState& state = getLaneState(laneName);

    return state.arrivalDist(state.rng) < state.arrivalRate;
}

Vehicle VehicleGenerator::generateVehicle(
    const std::string& laneName,
    int currentTick
) {
    LaneRandomState& state = getLaneState(laneName);

    return generateVehicleFromState(state, currentTick);
}

// --------------------------------------------------
// INTERSECTION-AWARE METHODS
// --------------------------------------------------

void VehicleGenerator::configureLane(
    const std::string& intersectionId,
    const std::string& laneName,
    double arrivalRate
) {
    validateProbability(arrivalRate, "Lane arrival rate");

    const std::string key =
        makeLaneKey(intersectionId, laneName);

    getLaneState(key).arrivalRate = arrivalRate;
}

bool VehicleGenerator::shouldGenerate(
    const std::string& intersectionId,
    const std::string& laneName
) {
    const std::string key =
        makeLaneKey(intersectionId, laneName);

    LaneRandomState& state = getLaneState(key);

    return state.arrivalDist(state.rng) < state.arrivalRate;
}

Vehicle VehicleGenerator::generateVehicle(
    const std::string& intersectionId,
    const std::string& laneName,
    int currentTick
) {
    const std::string key =
        makeLaneKey(intersectionId, laneName);

    LaneRandomState& state = getLaneState(key);

    return generateVehicleFromState(state, currentTick);
}

// --------------------------------------------------
// VEHICLE CREATION
// --------------------------------------------------

Vehicle VehicleGenerator::generateVehicleFromState(
    LaneRandomState& state,
    int currentTick
) {
    if (currentTick < 0) {
        throw std::invalid_argument(
            "Current tick cannot be negative"
        );
    }

    if (nextVehicleId_ >
        std::numeric_limits<int>::max()) {

        throw std::overflow_error(
            "Vehicle ID limit has been reached"
        );
    }

    VehicleType type =
        (state.emergencyDist(state.rng) <
         emergencyProbability_)
        ? VehicleType::EMERGENCY
        : VehicleType::NORMAL;

    const int vehicleId =
        static_cast<int>(nextVehicleId_);

    ++nextVehicleId_;

    return Vehicle(vehicleId, currentTick, type);
}
