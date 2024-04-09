#pragma once

#include <string>

// Keep this global
inline const char* smyt {"smyt: "};

namespace logging {
    void initialize();
    void uninitialize();
    void log(const std::string& message, bool flush = true);
}
