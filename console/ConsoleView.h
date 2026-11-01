// ConsoleView.h
// Renders live per-lane queue length and signal state to the terminal
// each tick. Satisfies TMS-NF-004 (Usability: readable live output).
#pragma once

#include <vector>
#include "LaneQueue.h"
#include "SignalController.h"

class ConsoleView {
public:
    void renderTick(int tick,
                     const std::vector<LaneQueue>& lanes,
                     const SignalController& signals) const;
};
