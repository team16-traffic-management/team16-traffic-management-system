
#include "Vehicle.h"

#include <stdexcept>

Vehicle::Vehicle(
    int id,
    int arrivalTick,
    VehicleType type
)
    : id_(id),
      arrivalTick_(arrivalTick),
      type_(type) {

    if (id_ <= 0) {
        throw std::invalid_argument(
            "Vehicle ID must be positive"
        );
    }

    if (arrivalTick_ < 0) {
        throw std::invalid_argument(
            "Vehicle arrival tick cannot be negative"
        );
    }

    if (type_ != VehicleType::NORMAL &&
        type_ != VehicleType::EMERGENCY) {
        throw std::invalid_argument(
            "Invalid vehicle type"
        );
    }
}

int Vehicle::getId() const {
    return id_;
}

int Vehicle::getArrivalTick() const {
    return arrivalTick_;
}

VehicleType Vehicle::getType() const {
    return type_;
}

bool Vehicle::isEmergency() const {
    return type_ == VehicleType::EMERGENCY;
}
