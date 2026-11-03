#ifndef TRAFFIC_SIGNAL_CONTROLLER_H
#define TRAFFIC_SIGNAL_CONTROLLER_H

#include <string>
#include <vector>
#include <map>
#include <cstddef>

enum class SignalPhase {
    RED,
    YELLOW,
    GREEN
};

class SignalController {
public:
    SignalController(
        int greenDuration = 10,
        int yellowDuration = 3,
        int redDuration = 10
    );

    void addLane(const std::string& laneName);

    void updateSignalState(int tickDelta);

    SignalPhase getCurrentPhase(
        const std::string& laneName
    ) const;

    // Returns true only when an emergency request is accepted.
    bool preempt(const std::string& laneName);

    void setAdaptiveMode(
        bool enabled,
        int maxExtension = 15
    );

    bool isAdaptiveMode() const;

    void updateAdaptiveTimers(
        const std::map<std::string, size_t>& laneQueueSizes
    );

private:
    int greenDuration_;
    int yellowDuration_;
    int redDuration_;

    int phaseTimer_{0};

    size_t activeLaneIndex_{0};

    SignalPhase currentPhase_{SignalPhase::GREEN};

    std::vector<std::string> lanes_;

    bool adaptiveMode_{false};
    int maxExtension_{15};

    bool preemptionPending_{false};
    std::string pendingEmergencyLane_;
};

#endif
