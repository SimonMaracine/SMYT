#pragma once

#include <cstdint>
#include <string>
#include <ctime>
#include <optional>
#include <vector>
#include <cstddef>

namespace helpers {
    std::uint16_t ntoh(std::uint16_t x);
    std::uint32_t ntoh(std::uint32_t x);
    std::string ntop(const void* x);
    std::optional<std::uint32_t> pton(const char* x);
    std::string ts(const std::time_t* x);
    bool most_common(const std::vector<std::uint32_t>& x, std::uint32_t& out_result, std::size_t& out_frequency);
}
