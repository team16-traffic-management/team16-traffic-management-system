#include "../include/ConsoleView.h"
#include <iostream>
#include <iomanip>
#include <string>

void ConsoleView::renderTick(int tick, const std::string& intersectionId,
                             const std::vector<LaneQueue>& lanes,
                             const SignalController& signals, bool silent) const {
    if (silent) return;
    std::cout << "\n[Intersection " << intersectionId << " | Tick "
              << std::setw(6) << tick << "]\n";
    std::cout << "  " << std::left << std::setw(14) << "Lane"
              << " | " << std::setw(8) << "Signal"
              << " | " << std::setw(7) << "Queue" << " | Vehicles\n";
    std::cout << "  " << std::string(58, '-') << "\n";
    for (const auto& lane : lanes) {
        const SignalPhase phase = signals.getCurrentPhase(lane.name());
        std::string phaseStr;
        switch (phase) {
            case SignalPhase::GREEN: phaseStr = "GREEN"; break;
            case SignalPhase::YELLOW: phaseStr = "YELLOW"; break;
            case SignalPhase::RED: phaseStr = "RED"; break;
        }
        const std::string visualBar(lane.size(), '#');
        std::cout << "  " << std::left << std::setw(14) << lane.name()
                  << " | " << std::setw(8) << phaseStr
                  << " | " << std::setw(7) << lane.size()
                  << " | " << visualBar << "\n";
    }
    std::cout.flush();
}
