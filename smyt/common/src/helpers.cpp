#include "helpers.hpp"

#include <unordered_map>
#include <algorithm>

#include <arpa/inet.h>

// ntop https://man7.org/linux/man-pages/man3/inet_ntop.3.html
// pton https://man7.org/linux/man-pages/man3/inet_pton.3.html
// max_element https://en.cppreference.com/w/cpp/algorithm/max_element

namespace helpers {
    std::uint16_t ntoh(std::uint16_t x) {
        return ntohs(x);
    }

    std::uint32_t ntoh(std::uint32_t x) {
        return ntohl(x);
    }

    std::optional<std::string> ntop(const void* x) {
        char result[32u] {};

        if (!inet_ntop(AF_INET, x, result, INET_ADDRSTRLEN)) {
            return std::nullopt;
        }

        return result;
    }

    std::optional<std::uint32_t> pton(const char* x) {
        std::uint32_t result {};

        if (!inet_pton(AF_INET, x, &result)) {
            return std::nullopt;
        }

        return result;
    }

    std::string ts(const std::time_t* x) {
        return std::asctime(std::localtime(x));
    }

    bool most_common(const std::vector<std::uint32_t>& x, std::uint32_t& out_result, std::size_t& out_frequency) {
        if (x.empty()) {
            return false;
        }

        std::unordered_map<std::uint32_t, std::size_t> frequency;

        for (const auto item : x) {
            frequency[item]++;
        }

        const auto result {
            std::max_element(frequency.begin(), frequency.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second < rhs.second;
            })
        };

        out_result = result->first;
        out_frequency = result->second;

        return true;
    }
}
