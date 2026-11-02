
#ifndef LANE_QUEUE_H
#define LANE_QUEUE_H

#include "Vehicle.h"

#include <cstddef>
#include <deque>
#include <string>

class LaneQueue {
private:
    std::deque<Vehicle> queue_;
    std::size_t capacity_;
    std::string name_;

public:
    // SAD-compatible constructor
    explicit LaneQueue(std::size_t maxCapacity);

    // Existing constructor retained for compatibility
    LaneQueue(std::string name, std::size_t maxCapacity = 20);

    bool enqueue(const Vehicle& vehicle);
    Vehicle dequeue();

    bool isFull() const;
    bool isEmpty() const;
    bool hasEmergency() const;

    std::size_t getQueueLength() const;

    // Existing methods retained for compatibility
    const std::string& name() const;
    std::size_t size() const;
    std::size_t capacity() const;
    const std::deque<Vehicle>& getQueue() const;
};

#endif
