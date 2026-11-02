
#ifndef VEHICLE_H
#define VEHICLE_H

enum class VehicleType {
    NORMAL,
    EMERGENCY
};

class Vehicle {
public:
    Vehicle(
        int id,
        int arrivalTick,
        VehicleType type = VehicleType::NORMAL
    );

    int getId() const;
    int getArrivalTick() const;
    VehicleType getType() const;
    bool isEmergency() const;

private:
    int id_;
    int arrivalTick_;
    VehicleType type_;
};

#endif
