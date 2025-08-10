#pragma once

#include <cstdint>

namespace Aurora::Platform {

struct TimePoint {
    uint64_t ticks;
};

uint64_t getTicksPerSecond();
TimePoint getTimeNow();
double secondsSince(const TimePoint& start);

}


