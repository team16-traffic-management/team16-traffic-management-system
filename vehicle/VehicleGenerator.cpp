#include "VehicleGenerator.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

VehicleGenerator::VehicleGenerator(const std::vector<double>& laneArrivalRates,
                                   std::uint32_t seed)
    : arrivalRates_(laneArrivalRates),
      engine_(seed),
      distribution_(0.0, 1.0),
      nextVehicleId_(new std::atomic<std::uint64_t>(1)),
      emergencyProbability_(0.05),
      defaultArrivalRate_(0.4),
      baseSeed_(seed),
      laneStates_() {
    if (arrivalRates_.empty()) {
        throw std::invalid_argument("At least one lane is required");
    }
    for (double probability : arrivalRates_) {
        validateProbability(probability);
    }
}

VehicleGenerator::VehicleGenerator(std::initializer_list<double> laneArrivalRates,
                                   std::uint32_t seed)
    : VehicleGenerator(std::vector<double>(laneArrivalRates), seed) {}

VehicleGenerator::VehicleGenerator(double arrivalRate,
                                   double emergencyProbability,
                                   unsigned int seed)
    : arrivalRates_(),
      engine_(seed),
      distribution_(0.0, 1.0),
      nextVehicleId_(new std::atomic<std::uint64_t>(1001)),
      emergencyProbability_(emergencyProbability),
      defaultArrivalRate_(arrivalRate),
      baseSeed_(seed),
      laneStates_() {
    validateProbability(arrivalRate);
    validateProbability(emergencyProbability);
}

void VehicleGenerator::setEmergencyProbability(double probability) {
    validateProbability(probability);
    emergencyProbability_ = probability;
}

double VehicleGenerator::getEmergencyProbability() const {
    return emergencyProbability_;
}

std::vector<Vehicle> VehicleGenerator::generateForTick(std::uint64_t tickValue) {
    std::vector<Vehicle> generated;
    for (std::size_t laneIndex = 0; laneIndex < arrivalRates_.size(); ++laneIndex) {
        if (distribution_(engine_) < arrivalRates_[laneIndex]) {
            const VehicleType type =
                distribution_(engine_) < emergencyProbability_
                    ? VehicleType::EMERGENCY
                    : VehicleType::NORMAL;
            generated.push_back(
                Vehicle(allocateVehicleId(), laneIndex, tickValue, type)
            );
        }
    }
    return generated;
}

void VehicleGenerator::setArrivalRate(std::size_t laneIndex, double probability) {
    if (laneIndex >= arrivalRates_.size()) {
        throw std::out_of_range("Lane index is out of range");
    }
    validateProbability(probability);
    arrivalRates_[laneIndex] = probability;
}

void VehicleGenerator::setArrivalRates(const std::vector<double>& probabilities) {
    if (probabilities.size() != arrivalRates_.size()) {
        throw std::invalid_argument("Arrival-rate count must match lane count");
    }
    for (double probability : probabilities) {
        validateProbability(probability);
    }
    arrivalRates_ = probabilities;
}

double VehicleGenerator::getArrivalRate(std::size_t laneIndex) const {
    if (laneIndex >= arrivalRates_.size()) {
        throw std::out_of_range("Lane index is out of range");
    }
    return arrivalRates_[laneIndex];
}

std::size_t VehicleGenerator::getLaneCount() const {
    return arrivalRates_.size();
}

void VehicleGenerator::configureLane(const std::string& laneName, double arrivalRate) {
    validateProbability(arrivalRate);
    if (laneName.empty()) {
        throw std::invalid_argument("Lane name cannot be empty");
    }
    getLaneState(laneName).arrivalRate = arrivalRate;
}

bool VehicleGenerator::shouldGenerate(const std::string& laneName) {
    LaneRandomState& state = getLaneState(laneName);
    return state.arrivalDist(state.rng) < state.arrivalRate;
}

Vehicle VehicleGenerator::generateVehicle(const std::string& laneName, int currentTick) {
    LaneRandomState& state = getLaneState(laneName);
    return generateVehicleFromState(state, currentTick, laneName);
}

void VehicleGenerator::configureLane(const std::string& intersectionId,
                                     const std::string& laneName,
                                     double arrivalRate) {
    validateProbability(arrivalRate);
    getLaneState(makeLaneKey(intersectionId, laneName)).arrivalRate = arrivalRate;
}

bool VehicleGenerator::shouldGenerate(const std::string& intersectionId,
                                      const std::string& laneName) {
    LaneRandomState& state =
        getLaneState(makeLaneKey(intersectionId, laneName));
    return state.arrivalDist(state.rng) < state.arrivalRate;
}

Vehicle VehicleGenerator::generateVehicle(const std::string& intersectionId,
                                           const std::string& laneName,
                                           int currentTick) {
    LaneRandomState& state =
        getLaneState(makeLaneKey(intersectionId, laneName));
    return generateVehicleFromState(state, currentTick, laneName);
}

std::uint64_t VehicleGenerator::allocateVehicleId() {
    std::uint64_t current = nextVehicleId_->load();
    for (;;) {
        if (current == std::numeric_limits<std::uint64_t>::max()) {
            throw std::overflow_error("Vehicle ID limit has been reached");
        }
        if (nextVehicleId_->compare_exchange_weak(current, current + 1)) {
            return current;
        }
    }
}

void VehicleGenerator::validateProbability(double probability) {
    if (!std::isfinite(probability) || probability < 0.0 || probability > 1.0) {
        throw std::invalid_argument("Arrival probability must be between 0 and 1");
    }
}

std::string VehicleGenerator::makeLaneKey(const std::string& intersectionId,
                                          const std::string& laneName) {
    if (intersectionId.empty() || laneName.empty()) {
        throw std::invalid_argument("Intersection ID and lane name cannot be empty");
    }
    return intersectionId + "::" + laneName;
}

VehicleGenerator::LaneRandomState& VehicleGenerator::getLaneState(const std::string& laneKey) {
    if (laneKey.empty()) {
        throw std::invalid_argument("Lane key cannot be empty");
    }
    std::unordered_map<std::string, LaneRandomState>::iterator it =
        laneStates_.find(laneKey);
    if (it == laneStates_.end()) {
        unsigned int laneSeed = baseSeed_;
        for (std::string::const_iterator ch = laneKey.begin(); ch != laneKey.end(); ++ch) {
            laneSeed = laneSeed * 31u + static_cast<unsigned char>(*ch);
        }
        it = laneStates_.emplace(std::piecewise_construct,
                                 std::forward_as_tuple(laneKey),
                                 std::forward_as_tuple(defaultArrivalRate_, laneSeed)).first;
    }
    return it->second;
}

Vehicle VehicleGenerator::generateVehicleFromState(LaneRandomState& state,
                                                    int currentTick,
                                                    const std::string& laneName) {
    if (currentTick < 0) {
        throw std::invalid_argument("Current tick cannot be negative");
    }
    const VehicleType type =
        state.emergencyDist(state.rng) < emergencyProbability_
            ? VehicleType::EMERGENCY
            : VehicleType::NORMAL;
    const std::uint64_t newId = allocateVehicleId();
    if (newId > static_cast<std::uint64_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error("Vehicle ID exceeds supported integer range");
    }
    Vehicle vehicle(static_cast<int>(newId), currentTick, type);
    vehicle.laneName = laneName;
    return vehicle;
}
