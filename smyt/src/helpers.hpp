#pragma once

#include <cstdint>
#include <string>
#include <ctime>

namespace helpers {
    std::uint16_t ntoh(std::uint16_t x);
    std::string ntop(const void* x);
    std::string ts(const std::time_t* x);
}
