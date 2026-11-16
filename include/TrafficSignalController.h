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
    // Ticks of all-red clearance inserted between the Yellow phase of the
    // interrupted lane and the Green phase granted to an emergency lane.
    static constexpr int kPreemptionAllRedTicks = 1;

    SignalController(
        int greenDuration = 10,
        int yellowDuration = 3,
        int redDuration = 10
    );

    void addLane(const std::string& laneName);

    // Replaces the configured base durations (all must be positive). The
    // adaptive-mode baseline follows the new Green duration.
    void setDurations(int greenDuration, int yellowDuration, int redDuration);

    void updateSignalState(int tickDelta);

    SignalPhase getCurrentPhase(
        const std::string& laneName
    ) const;

    // Returns true only when an emergency request is accepted. An accepted
    // request ends the current Green early, always runs the full Yellow
    // phase plus kPreemptionAllRedTicks of all-red, then grants Green to the
    // emergency lane: worst-case delay = yellowDuration + kPreemptionAllRedTicks.
    bool preempt(const std::string& laneName);

    bool isPreemptionPending() const;

    // Configured upper bound on the emergency preemption delay (ticks). It is
    // rejected when it is shorter than the guaranteed worst-case delay, so a
    // configuration can never promise a bound the controller cannot honour.
    void setMaxPreemptionDelay(int ticks);
    int getMaxPreemptionDelay() const;
    int worstCasePreemptionDelay() const;

    void setAdaptiveMode(
        bool enabled,
        int maxExtension = 15
    );

    bool isAdaptiveMode() const;
    int getMaxGreenExtension() const;

    void updateAdaptiveTimers(
        const std::map<std::string, size_t>& laneQueueSizes
    );

    int getGreenDuration() const;
    int getBaseGreenDuration() const;
    int getYellowDuration() const;
    int getRedDuration() const;

private:
    int baseGreenDuration_;
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
    int maxPreemptionDelay_;

    void advancePreemption();
};

#endif
