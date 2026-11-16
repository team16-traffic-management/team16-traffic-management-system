#include "../include/ConsoleView.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>

void ConsoleView::renderTick(int tick, const std::string& intersectionId,
                             const std::vector<LaneQueue>& lanes,
                             const SignalController& signals, bool silent) const {
    if (silent) return;

    // Size the lane column to the longest lane name so long names never push
    // the Signal/Queue columns out of alignment (TMS-NF-004).
    std::size_t laneWidth = 14;
    for (const auto& lane : lanes) laneWidth = std::max(laneWidth, lane.name().size());
    const int laneColumn = static_cast<int>(laneWidth);

    std::cout << "\n[Intersection " << intersectionId << " | Tick "
              << std::setw(6) << tick << "]\n";
    std::cout << "  " << std::left << std::setw(laneColumn) << "Lane"
              << " | " << std::setw(8) << "Signal"
              << " | " << std::setw(7) << "Queue" << " | Vehicles\n";
    std::cout << "  " << std::string(laneWidth + 44, '-') << "\n";
    for (const auto& lane : lanes) {
        const SignalPhase phase = signals.getCurrentPhase(lane.name());
        std::string phaseStr;
        switch (phase) {
            case SignalPhase::GREEN: phaseStr = "GREEN"; break;
            case SignalPhase::YELLOW: phaseStr = "YELLOW"; break;
            case SignalPhase::RED: phaseStr = "RED"; break;
        }
        const std::string visualBar(lane.size(), '#');
        std::cout << "  " << std::left << std::setw(laneColumn) << lane.name()
                  << " | " << std::setw(8) << phaseStr
                  << " | " << std::setw(7) << lane.size()
                  << " | " << visualBar << "\n";
    }
    std::cout.flush();
}
