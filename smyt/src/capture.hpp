#pragma once

#include <string>

namespace capture {
    void initialize();
    void uninitialize();

    void start_session(const std::string& device);
    void stop_session();

    void capture_loop();
    void break_loop();
}
