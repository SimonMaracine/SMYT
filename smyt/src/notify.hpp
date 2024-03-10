#pragma once

#include <string>
#include <optional>

namespace notify {
    void initialize();
    void uninitialize();
    void notify(const std::string& summary, const std::optional<std::string>& body);
}
