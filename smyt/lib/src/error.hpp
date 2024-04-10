#pragma once

#include <stdexcept>
#include <string>

namespace error {
    struct PcapError : public std::runtime_error {
        explicit PcapError(const std::string& message)
            : std::runtime_error(message) {}

        explicit PcapError(const char* message)
            : std::runtime_error(message) {}
    };

    struct LogError : public std::runtime_error {
        explicit LogError(const std::string& message)
            : std::runtime_error(message) {}

        explicit LogError(const char* message)
            : std::runtime_error(message) {}
    };

    struct ArgsError : public std::runtime_error {
        explicit ArgsError(const std::string& message)
            : std::runtime_error(message) {}

        explicit ArgsError(const char* message)
            : std::runtime_error(message) {}
    };

    struct ConfigError : public std::runtime_error {
        explicit ConfigError(const std::string& message)
            : std::runtime_error(message) {}

        explicit ConfigError(const char* message)
            : std::runtime_error(message) {}
    };
}
