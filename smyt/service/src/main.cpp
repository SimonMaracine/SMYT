#include <iostream>
#include <csignal>
#include <optional>
#include <string>
#include <cassert>

#include "capture.hpp"
#include "error.hpp"
#include "logging.hpp"
#include "configuration.hpp"
#include "daemon.hpp"

static void signal_handler(int) {
    // Should be fine to call this
    capture::break_loop();
}

static std::optional<std::string> choose_device(
    const configuration::Config& config,
    const std::optional<capture::Device>& default_device
) {
    if (!config.device.empty()) {
        return config.device;
    }

    if (default_device) {
        return default_device->name;
    }

    return std::nullopt;
}

int main() {
    if (std::signal(SIGTERM, signal_handler) == SIG_ERR) {
        sdaemon::notify_on_error("%s%s", smyt, "Could not setup terminate handler");
        return 1;
    }

    configuration::Config config;

    try {
        configuration::load(config);
    } catch (const error::ConfigError&) {}

    try {
        logging::initialize();
    } catch (const error::LogError& e) {
        sdaemon::notify_on_error("%s%s", smyt, e.what());
        return 1;
    }

    try {
        logging::log("Starting capture");
    } catch (const error::LogError&) {}

    std::optional<capture::Device> default_device;
    std::optional<std::string> device;

    try {
        default_device = capture::initialize();
    } catch (const error::PcapError& e) {
        sdaemon::notify_on_error("%s%s", smyt, e.what());
        goto error_logging;
    }

    device = choose_device(config, default_device);

    if (!device) {
        sdaemon::notify_on_error("%s%s", smyt, "No device to capture on");
        goto error_logging;
    }

    try {
        capture::create_session(*device);
    } catch (const error::PcapError& e) {
        sdaemon::notify_on_error("%s%s", smyt, e.what());
        goto error_logging;
    }

    sdaemon::notify_ready();

    try {
        capture::capture_loop(config, nullptr);
    } catch (const error::PcapError& e) {
        sdaemon::notify_on_error("%s%s", smyt, e.what());
        goto error_capture;
    }

    try {
        logging::log("Ending capture");
    } catch (const error::LogError&) {}

    capture::destroy_session();
    logging::uninitialize();

    sdaemon::notify_stopping();

    return 0;

error_capture:
    capture::destroy_session();

error_logging:
    logging::uninitialize();

    sdaemon::notify_stopping();

    return 1;
}
