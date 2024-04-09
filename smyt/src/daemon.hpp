#pragma once

namespace sdaemon {
    void notify_ready();
    void notify_stopping();
    void notify_on_error(const char* format, ...);
}
