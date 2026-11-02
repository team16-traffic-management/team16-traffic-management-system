#include "Vehicle.h"

#include <limits>
#include <stdexcept>

Vehicle::Vehicle(std::uint64_t vehicleId,
                 std::size_t laneIndex,
                 std::uint64_t arrivalTick,
                 VehicleType vehicleType)
    : id(vehicleId),
      lane(laneIndex),
      tick(arrivalTick),
      laneName(),
      type_(vehicleType) {
    if (id == 0) {
        throw std::invalid_argument("Vehicle ID must be positive");
    }
    if (type_ != VehicleType::NORMAL && type_ != VehicleType::EMERGENCY) {
        throw std::invalid_argument("Invalid vehicle type");
    }
}

Vehicle::Vehicle(int vehicleId, int arrivalTick, VehicleType vehicleType)
    : id(vehicleId > 0 ? static_cast<std::uint64_t>(vehicleId) : 0),
      lane(0),
      tick(arrivalTick >= 0 ? static_cast<std::uint64_t>(arrivalTick) : 0),
      laneName(),
      type_(vehicleType) {
    if (vehicleId <= 0) {
        throw std::invalid_argument("Vehicle ID must be positive");
    }
    if (arrivalTick < 0) {
        throw std::invalid_argument("Vehicle arrival tick cannot be negative");
    }
    if (type_ != VehicleType::NORMAL && type_ != VehicleType::EMERGENCY) {
        throw std::invalid_argument("Invalid vehicle type");
    }
}

int Vehicle::getId() const {
    return static_cast<int>(id);
}

int Vehicle::getArrivalTick() const {
    if (tick > static_cast<std::uint64_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error("Arrival tick exceeds supported integer range");
    }
    return static_cast<int>(tick);
}

VehicleType Vehicle::getType() const {
    return type_;
}

bool Vehicle::isEmergency() const {
    return type_ == VehicleType::EMERGENCY;
}
