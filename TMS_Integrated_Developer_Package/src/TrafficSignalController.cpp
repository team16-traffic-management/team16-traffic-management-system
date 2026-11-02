
#include "TrafficSignalController.h"

#include <algorithm>
#include <stdexcept>

SignalController::SignalController(
    int greenDuration,
    int yellowDuration,
    int redDuration
)
    : greenDuration_(greenDuration),
      yellowDuration_(yellowDuration),
      redDuration_(redDuration) {

    if (greenDuration <= 0 ||
        yellowDuration <= 0 ||
        redDuration <= 0) {

        throw std::invalid_argument(
            "Signal durations must be positive"
        );
    }
}

void SignalController::addLane(
    const std::string& laneName
) {
    if (laneName.empty()) {
        throw std::invalid_argument(
            "Lane name cannot be empty"
        );
    }

    if (std::find(
            lanes_.begin(),
            lanes_.end(),
            laneName
        ) != lanes_.end()) {

        throw std::invalid_argument(
            "Duplicate lane name"
        );
    }

    lanes_.push_back(laneName);
}

void SignalController::updateSignalState(
    int tickDelta
) {
    if (lanes_.empty() || tickDelta <= 0) {
        return;
    }

    phaseTimer_ += tickDelta;

    switch (currentPhase_) {

        case SignalPhase::GREEN:

            if (phaseTimer_ >= greenDuration_) {
                currentPhase_ = SignalPhase::YELLOW;
                phaseTimer_ = 0;
            }

            break;

        case SignalPhase::YELLOW:

            if (phaseTimer_ >= yellowDuration_) {
                currentPhase_ = SignalPhase::RED;
                phaseTimer_ = 0;
            }

            break;

        case SignalPhase::RED:

            if (phaseTimer_ >= redDuration_) {

                if (preemptionPending_) {

                    auto it = std::find(
                        lanes_.begin(),
                        lanes_.end(),
                        pendingEmergencyLane_
                    );

                    if (it != lanes_.end()) {
                        activeLaneIndex_ =
                            static_cast<size_t>(
                                std::distance(
                                    lanes_.begin(),
                                    it
                                )
                            );
                    } else {
                        activeLaneIndex_ =
                            (activeLaneIndex_ + 1) %
                            lanes_.size();
                    }

                    preemptionPending_ = false;
                    pendingEmergencyLane_.clear();

                } else {

                    activeLaneIndex_ =
                        (activeLaneIndex_ + 1) %
                        lanes_.size();
                }

                currentPhase_ = SignalPhase::GREEN;
                phaseTimer_ = 0;
            }

            break;
    }
}

SignalPhase SignalController::getCurrentPhase(
    const std::string& laneName
) const {

    if (lanes_.empty()) {
        return SignalPhase::RED;
    }

    if (lanes_[activeLaneIndex_] == laneName) {
        return currentPhase_;
    }

    return SignalPhase::RED;
}

bool SignalController::preempt(
    const std::string& laneName
) {
    auto it = std::find(
        lanes_.begin(),
        lanes_.end(),
        laneName
    );

    // Reject requests for unknown lanes.
    if (it == lanes_.end()) {
        return false;
    }

    // Do not replace an existing emergency request.
    if (preemptionPending_) {
        return false;
    }

    // The active lane already has the signal priority.
    if (lanes_[activeLaneIndex_] == laneName) {
        return false;
    }

    // Accept the emergency request.
    preemptionPending_ = true;
    pendingEmergencyLane_ = laneName;

    return true;
}

void SignalController::setAdaptiveMode(
    bool enabled,
    int maxExtension
) {
    if (maxExtension < 0) {
        throw std::invalid_argument(
            "Maximum green extension cannot be negative"
        );
    }

    adaptiveMode_ = enabled;
    maxExtension_ = maxExtension;
}

bool SignalController::isAdaptiveMode() const {
    return adaptiveMode_;
}

void SignalController::updateAdaptiveTimers(
    const std::map<std::string, size_t>& laneQueueSizes
) {
    if (!adaptiveMode_ ||
        lanes_.empty() ||
        currentPhase_ != SignalPhase::GREEN) {

        return;
    }

    const std::string& activeLane =
        lanes_[activeLaneIndex_];

    auto it = laneQueueSizes.find(activeLane);

    if (it == laneQueueSizes.end()) {
        return;
    }

    if (it->second > 5) {

        greenDuration_ = std::min(
            greenDuration_ + 2,
            10 + maxExtension_
        );

    } else {

        greenDuration_ = 10;
    }
}
