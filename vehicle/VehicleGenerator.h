#ifndef VEHICLE_GENERATOR_H
#define VEHICLE_GENERATOR_H

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

struct Vehicle {
    std::uint64_t id;
    std::size_t lane;
    std::uint64_t tick;
};

class VehicleGenerator {
public:
    explicit VehicleGenerator(const std::vector<double>& laneArrivalRates,
                              std::uint32_t seed = std::random_device{}());

    std::vector<Vehicle> generateForTick(std::uint64_t tick);

    void setArrivalRate(std::size_t lane, double probability);
    void setArrivalRates(const std::vector<double>& probabilities);

    double getArrivalRate(std::size_t lane) const;
    std::size_t getLaneCount() const;

private:
    static void validateProbability(double probability);

    std::vector<double> arrivalRates_;
    std::mt19937 engine_;
    std::uniform_real_distribution<double> distribution_;
    std::uint64_t nextVehicleId_;
};

#endif
