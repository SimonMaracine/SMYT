#pragma once

namespace capture {
    void initialize();
    void uninitialize();

    void start_session();
    void stop_session();

    void capture_loop();
    void break_loop();
}
