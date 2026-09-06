// LaneQueue.h
// NOTE: This is a MOCK/STUB version so ConsoleView and StatisticsCollector
// can be developed and tested independently of Teammate 3's real
// LaneQueue.cpp task. Only the read-only interface below (name, size,
// capacity, isFull) needs to exist on the real class for your code to work
// unchanged — the mockPush/mockPop/mockFront helpers here are ONLY for the
// demo harness and won't exist on the real LaneQueue.
#pragma once

#include <deque>
#include <string>
#include "Vehicle.h"

class LaneQueue {
public:
    LaneQueue(std::string name, int capacity)
        : name_(std::move(name)), capacity_(capacity) {}

    const std::string& name() const { return name_; }
    int size() const { return static_cast<int>(vehicles_.size()); }
    int capacity() const { return capacity_; }
    bool isFull() const { return size() >= capacity_; }

    // ---- demo-only helpers, not part of the real LaneQueue interface ----
    void mockPush(const Vehicle& v) { if (!isFull()) vehicles_.push_back(v); }
    Vehicle mockFront() const { return vehicles_.front(); }
    void mockPop() { if (!vehicles_.empty()) vehicles_.pop_front(); }

private:
    std::string name_;
    int capacity_;
    std::deque<Vehicle> vehicles_;
};
