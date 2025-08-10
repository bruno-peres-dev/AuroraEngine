#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <utility>

namespace Aurora::Core {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

void initializeLogging();
void shutdownLogging();

void log(LogLevel level, std::string_view message);

template <typename... Args>
void logf(LogLevel level, std::string_view format, Args&&... args) {
    std::ostringstream oss;
    oss << format;
    ((oss << ' ' << std::forward<Args>(args)), ...);
    log(level, oss.str());
}

}


