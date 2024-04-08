#pragma once

#include <string>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "configuration.hpp"

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

    struct SynPacket {
        std::uint32_t src_address {};
        std::uint32_t dst_address {};
        long timestamp {};
    };

    struct SynScan {
        bool panic_mode {false};  // Scan in progress
        std::size_t syn_packet_count {};
        std::vector<SynPacket> syn_packets;
    };

    struct SessionData {
        SynScan scan;
        long last_process {};
        configuration::Config config;

        PacketCallback callback {nullptr};
    };

    std::optional<Device> initialize();

    void create_session(const std::string& device);
    void destroy_session();

    void capture_loop(const configuration::Config& config);
    void break_loop();

    const char* get_library_version();
}
