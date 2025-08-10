#include "Aurora/Core/Log.hpp"

#include <chrono>
#include <cstdio>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace Aurora::Core {

namespace {
    std::mutex g_logMutex;

    const char* toString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace: return "TRACE";
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warn: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            default: return "?";
        }
    }
}

void initializeLogging() {}
void shutdownLogging() {}

void log(LogLevel level, std::string_view message) {
    std::scoped_lock lock(g_logMutex);

    auto now = std::chrono::system_clock::now();
    auto t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#if defined(_WIN32)
    localtime_s(&tm_buf, &t_c);
#else
    localtime_r(&t_c, &tm_buf);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");

    std::fprintf(stderr, "[%s] [%s] %.*s\n", oss.str().c_str(), toString(level), (int)message.size(), message.data());
}

}


