#pragma once

#include <string>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <iosfwd>  // std::ostream
#include <thread>
#include <mutex>
#include <condition_variable>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include "configuration.hpp"

#define SMYT_LOG_PACKETS 0

namespace capture {
    struct SessionData;

    using PacketCallback = void(*)(
        long,
        std::size_t,
        const struct ether_header*,
        const struct ip*,
        const struct tcphdr*,
        SessionData&
    );

    struct Device {
        std::string name;
        std::string description;
    };

    struct SynPacket {
        std::uint32_t src_address {};
        std::uint32_t dst_address {};
        long timestamp {};
    };

    struct State {
        bool panic_mode {false};  // Scan in progress
        std::size_t syn_packet_count {};
        std::vector<SynPacket> syn_packets;

        std::vector<std::uint32_t> blocked_addresses;
    };

    struct SessionData {
        configuration::Config config;
        std::ostream* err_stream {nullptr};
        PacketCallback callback {nullptr};

        State state;
        long last_process {};
        std::thread thread;
        std::mutex mutex;
        std::condition_variable cv;
        bool processing {true};
    };

    std::optional<Device> initialize();

    void create_session(const std::string& device);
    void destroy_session();

    void capture_loop(const configuration::Config& config, std::ostream* err_stream);
    void break_loop();

    const char* get_library_version();
}
