#pragma once

namespace sdaemon {
    void notify_ready();
    void notify_stopping(bool set_status = true);
    void notify_on_error(const char* format, ...);
}
