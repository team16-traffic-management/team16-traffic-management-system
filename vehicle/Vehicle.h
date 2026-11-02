#ifndef VEHICLE_H
#define VEHICLE_H

#include <cstddef>
#include <cstdint>
#include <string>

enum class VehicleType {
    NORMAL,
    EMERGENCY
};

class Vehicle {
public:
    // Public fields preserve the interface used by the existing main.cpp/tests.
    std::uint64_t id;
    std::size_t lane;
    std::uint64_t tick;
    std::string laneName;

    Vehicle(std::uint64_t vehicleId,
            std::size_t laneIndex,
            std::uint64_t arrivalTick,
            VehicleType vehicleType = VehicleType::NORMAL);

    // Object-oriented constructor used by the integrated simulation modules.
    Vehicle(int vehicleId,
            int arrivalTick,
            VehicleType vehicleType = VehicleType::NORMAL);

    int getId() const;
    int getArrivalTick() const;
    VehicleType getType() const;
    bool isEmergency() const;

private:
    VehicleType type_;
};

#endif
