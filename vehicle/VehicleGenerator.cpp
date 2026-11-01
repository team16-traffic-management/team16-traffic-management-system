#include "VehicleGenerator.h"

#include <cmath>
#include <stdexcept>

VehicleGenerator::VehicleGenerator(const std::vector<double>& laneArrivalRates,
                                   std::uint32_t seed)
    : arrivalRates_(laneArrivalRates),
      engine_(seed),
      distribution_(0.0, 1.0),
      nextVehicleId_(1) {
    if (arrivalRates_.empty()) {
        throw std::invalid_argument("At least one lane is required");
    }

    for (double probability : arrivalRates_) {
        validateProbability(probability);
    }
}

std::vector<Vehicle> VehicleGenerator::generateForTick(std::uint64_t tick) {
    std::vector<Vehicle> generated;

    for (std::size_t lane = 0; lane < arrivalRates_.size(); ++lane) {
        if (distribution_(engine_) < arrivalRates_[lane]) {
            generated.push_back(Vehicle{nextVehicleId_++, lane, tick});
        }
    }

    return generated;
}

void VehicleGenerator::setArrivalRate(std::size_t lane, double probability) {
    if (lane >= arrivalRates_.size()) {
        throw std::out_of_range("Lane index is out of range");
    }
    validateProbability(probability);
    arrivalRates_[lane] = probability;
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

double VehicleGenerator::getArrivalRate(std::size_t lane) const {
    if (lane >= arrivalRates_.size()) {
        throw std::out_of_range("Lane index is out of range");
    }
    return arrivalRates_[lane];
}

std::size_t VehicleGenerator::getLaneCount() const {
    return arrivalRates_.size();
}

void VehicleGenerator::validateProbability(double probability) {
    if (!std::isfinite(probability) || probability < 0.0 || probability > 1.0) {
        throw std::invalid_argument("Arrival probability must be between 0 and 1");
    }
}
