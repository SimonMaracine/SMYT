#pragma once

#include <string>

namespace logging {
    void initialize();
    void uninitialize();
    void log(const std::string& message, bool flush = false);
}
