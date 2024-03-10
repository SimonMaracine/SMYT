#include "notify.hpp"

#include <libnotify/notify.h>
#include <libnotify/notification.h>

#include "error.hpp"

namespace notify {
    void initialize() {
        if (!notify_init("smyt")) {
            throw error::LibnotifyError("Could not initialize libnotify");
        }
    }

    void uninitialize() {
        notify_uninit();
    }

    void notify(const std::string& summary, const std::optional<std::string>& body) {
        NotifyNotification* notification {notify_notification_new(
            summary.c_str(),
            body ? body->c_str() : nullptr,
            nullptr)
        };

        GError* error {nullptr};

        if (!notify_notification_show(notification, &error)) {
            std::string err_msg {error->message};
            g_error_free(error);

            throw error::LibnotifyError("Could not show notification: " + err_msg);
        }
    }
}
