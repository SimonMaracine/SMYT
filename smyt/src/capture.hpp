#pragma once

#include <string>
#include <optional>
#include <cstddef>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

namespace capture {
    using PacketCallback = void(*)(long, std::size_t, const struct ether_header*, const struct ip*, const struct tcphdr*);

    struct Device {
        std::string name;
        std::string description;
        // TODO flags
    };

    std::optional<Device> initialize();
    void uninitialize();

    void start_session(const std::string& device);
    void stop_session();

    void capture_loop();
    void break_loop();

    const char* get_library_version();
}
