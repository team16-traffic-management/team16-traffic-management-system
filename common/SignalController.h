// SignalController.h
// NOTE: This is a MOCK/STUB version so ConsoleView can be developed and
// tested independently of Teammate 3's real SignalController.cpp task.
// Only getColor(laneName) needs to exist on the real class for ConsoleView
// to work unchanged — mockSetColor() here is ONLY for the demo harness.
#pragma once

#include <map>
#include <string>

enum class SignalColor { RED, YELLOW, GREEN };

class SignalController {
public:
    SignalColor getColor(const std::string& laneName) const {
        auto it = colors_.find(laneName);
        return it != colors_.end() ? it->second : SignalColor::RED;
    }

    // ---- demo-only helper, not part of the real SignalController interface ----
    void mockSetColor(const std::string& laneName, SignalColor c) { colors_[laneName] = c; }

private:
    std::map<std::string, SignalColor> colors_;
};
