#include "Aurora/Platform/Time.hpp"

#include <chrono>

namespace Aurora::Platform {

static uint64_t g_ticksPerSecond = 1'000'000'000ull; // nanoseconds

uint64_t getTicksPerSecond() {
    return g_ticksPerSecond;
}

TimePoint getTimeNow() {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return TimePoint{ static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count()) };
}

double secondsSince(const TimePoint& start) {
    TimePoint now = getTimeNow();
    uint64_t delta = now.ticks - start.ticks;
    return static_cast<double>(delta) / static_cast<double>(g_ticksPerSecond);
}

}


