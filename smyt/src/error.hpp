#pragma once

#include <stdexcept>
#include <string>

namespace error {
    struct PcapError : public std::runtime_error {
        explicit PcapError(const std::string& message)
            : std::runtime_error(message) {}
    };
}
