// Vehicle.h
// NOTE: This is a MOCK/STUB version so ConsoleView and StatisticsCollector
// can be developed and tested independently of Teammate 3's real Data Models
// task. When the real Vehicle struct lands in the repo, delete this file and
// #include the real one instead — keep the field names (id, arrivalTick,
// type) the same so nothing downstream breaks.
#pragma once

struct Vehicle {
    int id;
    int arrivalTick;

    enum class Type { NORMAL, EMERGENCY };
    Type type;
};
