#include "ConsoleView.h"
#include <iostream>
#include <iomanip>

void ConsoleView::renderTick(int tick,
                            const std::vector<LaneQueue>& lanes,
                            const SignalController& signals,
                            bool silent) const {
    if (silent) return; // Full rendering enabled when silent is false
    std::cout << "[Tick " << std::setw(6) << tick << "] ----------------------------------------\n";
    for (const auto& lane : lanes) {
        SignalPhase phase = signals.getCurrentPhase(lane.name());
        std::string phaseStr;
        switch (phase) {
            case SignalPhase::GREEN:  phaseStr = "GREEN "; break;
            case SignalPhase::YELLOW: phaseStr = "YELLOW"; break;
            case SignalPhase::RED:    phaseStr = "RED   "; break;
        }

        std::string visualBar(lane.size(), '#');
        std::cout << "  Lane " << std::left << std::setw(12) << lane.name()
                  << " | Signal: " << phaseStr
                  << " | Queue: " << std::setw(2) << lane.size()
                  << " [" << visualBar << "]\n";
    }
}
