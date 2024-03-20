#include "helpers.hpp"

#include <arpa/inet.h>

namespace helpers {
    std::uint16_t ntoh(std::uint16_t x) {
        return ntohs(x);
    }

    std::string ntop(const void* x) {
        char result[32u] {};

        inet_ntop(AF_INET, x, result, INET_ADDRSTRLEN);

        return result;
    }

    std::string ts(const std::time_t* x) {
        return std::asctime(std::localtime(x));
    }
}
