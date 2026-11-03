#include "../include/LaneQueue.h"

#include <stdexcept>
#include <utility>

// SAD-compatible constructor
LaneQueue::LaneQueue(std::size_t maxCapacity)
    : LaneQueue("Lane", maxCapacity) {}

// Existing constructor
LaneQueue::LaneQueue(
    std::string name,
    std::size_t maxCapacity
)
    : capacity_(maxCapacity),
      name_(std::move(name)) {

    if (name_.empty()) {
        throw std::invalid_argument(
            "Lane name cannot be empty"
        );
    }

    if (capacity_ == 0) {
        throw std::invalid_argument(
            "Lane capacity must be greater than zero"
        );
    }
}

bool LaneQueue::enqueue(const Vehicle& vehicle) {

    // Reject vehicles when the queue is full.
    if (isFull()) {
        return false;
    }

    // Give emergency vehicles priority.
    if (vehicle.isEmergency()) {

        auto it = queue_.begin();

        // Keep existing emergency vehicles ahead
        // of newly arriving emergency vehicles.
        while (it != queue_.end() &&
               it->isEmergency()) {
            ++it;
        }

        queue_.insert(it, vehicle);

    } else {

        // Normal vehicles follow FIFO order.
        queue_.push_back(vehicle);
    }

    return true;
}

Vehicle LaneQueue::dequeue() {

    if (isEmpty()) {
        throw std::runtime_error(
            "Cannot dequeue from empty LaneQueue: " + name_
        );
    }

    Vehicle vehicle = queue_.front();
    queue_.pop_front();

    return vehicle;
}

bool LaneQueue::isFull() const {
    return queue_.size() >= capacity_;
}

bool LaneQueue::isEmpty() const {
    return queue_.empty();
}

bool LaneQueue::hasEmergency() const {

    for (const auto& vehicle : queue_) {
        if (vehicle.isEmergency()) {
            return true;
        }
    }

    return false;
}

std::size_t LaneQueue::getQueueLength() const {
    return queue_.size();
}

const std::string& LaneQueue::name() const {
    return name_;
}

std::size_t LaneQueue::size() const {
    return queue_.size();
}

std::size_t LaneQueue::capacity() const {
    return capacity_;
}

const std::deque<Vehicle>& LaneQueue::getQueue() const {
    return queue_;
}
