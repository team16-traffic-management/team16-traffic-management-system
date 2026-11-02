
#include "Intersection.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

Intersection::Intersection(std::string id)
    : id_(std::move(id)) {

    if (id_.empty()) {
        throw std::invalid_argument(
            "Intersection ID cannot be empty"
        );
    }
}

void Intersection::addLane(
    const std::string& laneName,
    size_t capacity
) {
    if (laneName.empty()) {
        throw std::invalid_argument(
            "Lane name cannot be empty"
        );
    }

    if (capacity == 0) {
        throw std::invalid_argument(
            "Lane capacity must be greater than zero"
        );
    }

    auto existingLane = std::find_if(
        lanes_.begin(),
        lanes_.end(),
        [&laneName](const LaneQueue& lane) {
            return lane.name() == laneName;
        }
    );

    if (existingLane != lanes_.end()) {
        throw std::invalid_argument(
            "Duplicate lane name: " + laneName
        );
    }

    lanes_.emplace_back(laneName, capacity);
    signals_.addLane(laneName);
}

void Intersection::update(int tick) {
    (void)tick;
    signals_.updateSignalState(1);
}

const std::string& Intersection::getId() const {
    return id_;
}

std::vector<LaneQueue>& Intersection::getLanes() {
    return lanes_;
}

const std::vector<LaneQueue>& Intersection::getLanes() const {
    return lanes_;
}

SignalController& Intersection::getSignalController() {
    return signals_;
}

const SignalController& Intersection::getSignalController() const {
    return signals_;
}
