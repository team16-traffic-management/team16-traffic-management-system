#include "../include/TrafficSignalController.h"
#include <algorithm>
#include <limits>
#include <stdexcept>

SignalController::SignalController(int greenDuration, int yellowDuration, int redDuration)
    : baseGreenDuration_(greenDuration), greenDuration_(greenDuration),
      yellowDuration_(yellowDuration), redDuration_(redDuration) {
    if (greenDuration <= 0 || yellowDuration <= 0 || redDuration <= 0)
        throw std::invalid_argument("Signal durations must be positive");
}
void SignalController::addLane(const std::string& laneName) {
    if (laneName.empty()) throw std::invalid_argument("Lane name cannot be empty");
    if (std::find(lanes_.begin(), lanes_.end(), laneName) != lanes_.end())
        throw std::invalid_argument("Duplicate lane name");
    lanes_.push_back(laneName);
}
void SignalController::updateSignalState(int tickDelta) {
    if (lanes_.empty() || tickDelta <= 0) return;
    // Consume every complete phase interval and retain leftover elapsed ticks.
    long long remaining = tickDelta;
    while (remaining > 0) {
        int duration = 0;
        switch (currentPhase_) {
            case SignalPhase::GREEN: duration = greenDuration_; break;
            case SignalPhase::YELLOW: duration = yellowDuration_; break;
            case SignalPhase::RED: duration = redDuration_; break;
        }
        const long long needed = static_cast<long long>(duration) - phaseTimer_;
        if (remaining < needed) {
            phaseTimer_ += static_cast<int>(remaining);
            break;
        }
        remaining -= needed;
        phaseTimer_ = 0;
        if (currentPhase_ == SignalPhase::GREEN) currentPhase_ = SignalPhase::YELLOW;
        else if (currentPhase_ == SignalPhase::YELLOW) currentPhase_ = SignalPhase::RED;
        else {
            activeLaneIndex_ = (activeLaneIndex_ + 1) % lanes_.size();
            currentPhase_ = SignalPhase::GREEN;
        }
    }
}
SignalPhase SignalController::getCurrentPhase(const std::string& laneName) const {
    if (lanes_.empty()) return SignalPhase::RED;
    return lanes_[activeLaneIndex_] == laneName ? currentPhase_ : SignalPhase::RED;
}
bool SignalController::preempt(const std::string& laneName) {
    const auto it = std::find(lanes_.begin(), lanes_.end(), laneName);
    if (it == lanes_.end()) return false;
    const std::size_t requested = static_cast<std::size_t>(std::distance(lanes_.begin(), it));
    if (requested == activeLaneIndex_ && currentPhase_ == SignalPhase::GREEN) return false;
    // Immediate transfer of right-of-way; only one lane can be green.
    activeLaneIndex_ = requested;
    currentPhase_ = SignalPhase::GREEN;
    phaseTimer_ = 0;
    return true;
}
void SignalController::setAdaptiveMode(bool enabled, int maxExtension) {
    if (maxExtension < 0) throw std::invalid_argument("Maximum green extension cannot be negative");
    adaptiveMode_ = enabled;
    maxExtension_ = maxExtension;
    greenDuration_ = baseGreenDuration_;
}
bool SignalController::isAdaptiveMode() const { return adaptiveMode_; }
void SignalController::updateAdaptiveTimers(const std::map<std::string, size_t>& laneQueueSizes) {
    if (!adaptiveMode_ || lanes_.empty() || currentPhase_ != SignalPhase::GREEN) return;
    const auto it = laneQueueSizes.find(lanes_[activeLaneIndex_]);
    if (it == laneQueueSizes.end()) return;
    const long long extended = it->second > 5
        ? static_cast<long long>(baseGreenDuration_) + std::min<std::size_t>(it->second - 5, static_cast<std::size_t>(maxExtension_))
        : baseGreenDuration_;
    greenDuration_ = static_cast<int>(std::min<long long>(
        extended, static_cast<long long>(baseGreenDuration_) + maxExtension_));
}
