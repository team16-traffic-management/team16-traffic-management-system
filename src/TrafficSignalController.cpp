#include "../include/TrafficSignalController.h"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace {
void validateDurations(int greenDuration, int yellowDuration, int redDuration) {
    if (greenDuration <= 0 ||
        yellowDuration <= 0 ||
        redDuration <= 0) {

        throw std::invalid_argument(
            "Signal durations must be positive"
        );
    }
}
}

SignalController::SignalController(
    int greenDuration,
    int yellowDuration,
    int redDuration
)
    : baseGreenDuration_(greenDuration),
      greenDuration_(greenDuration),
      yellowDuration_(yellowDuration),
      redDuration_(redDuration),
      maxPreemptionDelay_(yellowDuration + kPreemptionAllRedTicks) {

    validateDurations(greenDuration, yellowDuration, redDuration);
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

void SignalController::setDurations(
    int greenDuration,
    int yellowDuration,
    int redDuration
) {
    validateDurations(greenDuration, yellowDuration, redDuration);

    baseGreenDuration_ = greenDuration;
    greenDuration_ = greenDuration;
    yellowDuration_ = yellowDuration;
    redDuration_ = redDuration;

    // Keep the preemption bound consistent with the new Yellow duration.
    maxPreemptionDelay_ = std::max(maxPreemptionDelay_, worstCasePreemptionDelay());
}

void SignalController::updateSignalState(
    int tickDelta
) {
    if (lanes_.empty() || tickDelta <= 0) {
        return;
    }

    phaseTimer_ += tickDelta;

    if (preemptionPending_) {
        advancePreemption();
        return;
    }

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
                activeLaneIndex_ =
                    (activeLaneIndex_ + 1) %
                    lanes_.size();

                currentPhase_ = SignalPhase::GREEN;
                phaseTimer_ = 0;
            }

            break;
    }
}

// Preemption sequence: end the current Green immediately, keep the full
// Yellow clearance, hold all-red for kPreemptionAllRedTicks, then grant
// Green to the emergency lane. At most one lane is ever Green.
void SignalController::advancePreemption() {
    switch (currentPhase_) {

        case SignalPhase::GREEN:
            currentPhase_ = SignalPhase::YELLOW;
            phaseTimer_ = 0;
            break;

        case SignalPhase::YELLOW:
            if (phaseTimer_ >= yellowDuration_) {
                currentPhase_ = SignalPhase::RED;
                phaseTimer_ = 0;
            }
            break;

        case SignalPhase::RED:
            if (phaseTimer_ >= kPreemptionAllRedTicks) {
                const auto it = std::find(
                    lanes_.begin(),
                    lanes_.end(),
                    pendingEmergencyLane_
                );

                activeLaneIndex_ = it != lanes_.end()
                    ? static_cast<size_t>(std::distance(lanes_.begin(), it))
                    : (activeLaneIndex_ + 1) % lanes_.size();

                preemptionPending_ = false;
                pendingEmergencyLane_.clear();
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

    // A lane that is already Green is being served; nothing to preempt.
    if (lanes_[activeLaneIndex_] == laneName &&
        currentPhase_ == SignalPhase::GREEN) {
        return false;
    }

    // Accept the emergency request.
    preemptionPending_ = true;
    pendingEmergencyLane_ = laneName;

    return true;
}

bool SignalController::isPreemptionPending() const {
    return preemptionPending_;
}

void SignalController::setMaxPreemptionDelay(int ticks) {
    if (ticks < worstCasePreemptionDelay()) {
        throw std::invalid_argument(
            "Maximum preemption delay must be at least yellow duration + " +
            std::to_string(kPreemptionAllRedTicks) + " tick(s) (" +
            std::to_string(worstCasePreemptionDelay()) + ")"
        );
    }

    maxPreemptionDelay_ = ticks;
}

int SignalController::getMaxPreemptionDelay() const {
    return maxPreemptionDelay_;
}

int SignalController::worstCasePreemptionDelay() const {
    return yellowDuration_ + kPreemptionAllRedTicks;
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

    if (!enabled) {
        greenDuration_ = baseGreenDuration_;
    }
}

bool SignalController::isAdaptiveMode() const {
    return adaptiveMode_;
}

int SignalController::getMaxGreenExtension() const {
    return maxExtension_;
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

    // Extend from the configured base Green duration, capped at
    // base + maxExtension; light queues restore the configured base.
    if (it->second > 5) {

        greenDuration_ = std::min(
            greenDuration_ + 2,
            baseGreenDuration_ + maxExtension_
        );

    } else {

        greenDuration_ = baseGreenDuration_;
    }
}

int SignalController::getGreenDuration() const { return greenDuration_; }
int SignalController::getBaseGreenDuration() const { return baseGreenDuration_; }
int SignalController::getYellowDuration() const { return yellowDuration_; }
int SignalController::getRedDuration() const { return redDuration_; }
