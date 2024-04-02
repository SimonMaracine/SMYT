#pragma once

#include <string>
#include <optional>
#include <cstddef>
#include <unordered_map>
#include <cstdint>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

namespace capture {
    struct SessionData;

    using PacketCallback = void(*)(
        long,
        std::size_t,
        const struct ether_header*,
        const struct ip*,
        const struct tcphdr*,
        SessionData*
    );

    struct Device {
        std::string name;
        std::string description;
        // TODO flags
    };

    struct TcpSession {
        std::uint32_t src_address {};
        long timestamp {};
    };

    struct SessionData {
        std::unordered_map<std::uint32_t, TcpSession> map;
        PacketCallback callback {nullptr};
    };

    std::optional<Device> initialize();

    void create_session(const std::string& device);
    void destroy_session();

    void capture_loop();
    void break_loop();

    const char* get_library_version();
}
