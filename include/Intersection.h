#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <cstddef>
#include <string>
#include <vector>

#include "LaneQueue.h"
#include "TrafficSignalController.h"

class Intersection {
public:
    explicit Intersection(std::string id);

    void addLane(
        const std::string& laneName,
        std::size_t capacity = 20
    );

    void update(int tick);

    const std::string& getId() const;

    std::vector<LaneQueue>& getLanes();
    const std::vector<LaneQueue>& getLanes() const;

    SignalController& getSignalController();
    const SignalController& getSignalController() const;

private:
    std::string id_;
    std::vector<LaneQueue> lanes_;
    SignalController signals_;
};

#endif
