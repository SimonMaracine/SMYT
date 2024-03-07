#pragma once

#include <string>
#include <optional>

namespace capture {
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
