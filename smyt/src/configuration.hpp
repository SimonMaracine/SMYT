#pragma once

#include <cstddef>

namespace configuration {
    struct Config {
        long process_period {10l};  // Seconds
        std::size_t warning_threshold {20u};  // SYN packets
        std::size_t panic_threshold {30u};
    };

    void load(Config& config);
}
