#pragma once

#include <stdexcept>
#include <string>

namespace error {
    struct PcapError : public std::runtime_error {
        explicit PcapError(const std::string& message)
            : std::runtime_error(message) {}
    };

    struct LibnotifyError : public std::runtime_error {
        explicit LibnotifyError(const std::string& message)
            : std::runtime_error(message) {}
    };

    struct ArgsError : public std::runtime_error {
        explicit ArgsError(const std::string& message)
            : std::runtime_error(message) {}
    };
}
