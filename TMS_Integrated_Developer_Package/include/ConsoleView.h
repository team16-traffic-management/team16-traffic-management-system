#ifndef CONSOLE_VIEW_H
#define CONSOLE_VIEW_H

#include <vector>
#include <string>
#include "LaneQueue.h"
#include "TrafficSignalController.h"

class ConsoleView {
public:
    void renderTick(int tick,
                    const std::vector<LaneQueue>& lanes,
                    const SignalController& signals,
                    bool silent = false) const;
};

#endif // CONSOLE_VIEW_H
