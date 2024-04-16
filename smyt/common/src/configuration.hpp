#pragma once

#include <cstddef>
#include <string>

namespace configuration {
    struct Config {
        long process_period {7l};  // Seconds
        std::size_t warning_threshold {20u};  // SYN packets
        std::size_t panic_threshold {30u};
        std::string device;
    };

    void load(Config& config);
}
