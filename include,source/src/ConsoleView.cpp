#include "ConsoleView.h"
#include <iostream>
#include <iomanip>
#include <string>

namespace {

const char* colorName(SignalColor c) {
    switch (c) {
        case SignalColor::GREEN:  return "GREEN ";
        case SignalColor::YELLOW: return "YELLOW";
        default:                  return "RED   ";
    }
}

} // namespace

void ConsoleView::renderTick(int tick,
                              const std::vector<LaneQueue>& lanes,
                              const SignalController& signals) const {
    std::cout << "---- Tick " << std::setw(5) << tick << " ----\n";
    for (const auto& lane : lanes) {
        SignalColor c = signals.getColor(lane.name());
        std::cout << std::left << std::setw(10) << lane.name()
                  << " | Signal: " << colorName(c)
                  << " | Queue: " << lane.size() << "/" << lane.capacity() << "  "
                  << std::string(lane.size(), '#') << "\n";
    }
    std::cout << std::endl;
}
