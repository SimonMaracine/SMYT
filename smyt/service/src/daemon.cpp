#include "daemon.hpp"

#include <cstdio>
#include <cstdarg>

#include <systemd/sd-daemon.h>

// https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html

namespace sdaemon {
    void notify_ready() {
        sd_notify(0, "READY=1\nSTATUS=Running");
    }

    void notify_stopping() {
        sd_notify(0, "STOPPING=1\nSTATUS=Stopped");
    }

    void notify_on_error(const char* format, ...) {
        va_list args;
        va_start(args, format);

        char buffer[256u] {};
        std::vsnprintf(buffer, sizeof(buffer), format, args);

        va_end(args);

        sd_notifyf(0, "STATUS=%s", buffer);
    }
}
