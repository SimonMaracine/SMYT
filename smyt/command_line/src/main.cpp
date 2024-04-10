#include <iostream>
#include <csignal>
#include <optional>
#include <string>
#include <cassert>

#include "capture.hpp"
#include "error.hpp"
#include "logging.hpp"
#include "configuration.hpp"
#include "args.hpp"

static void signal_handler(int) {
    // Should be fine to call this
    capture::break_loop();
}

static std::optional<std::string> choose_device(
    const args::Arguments& arguments,
    const std::optional<capture::Device>& default_device
) {
    if (!arguments.device.empty()) {
        return arguments.device;
    }

    if (default_device) {
        return default_device->name;
    }

    return std::nullopt;
}

static int capture_main(const args::Arguments& arguments) {
    if (std::signal(SIGINT, signal_handler) == SIG_ERR) {
        std::cerr << smyt << "Could not setup interrupt handler\n";
        return 1;
    }

    configuration::Config config;

    try {
        configuration::load(config);
    } catch (const error::ConfigError& e) {
        std::cerr << smyt << e.what() << '\n';
        std::cout << "Using default configuration\n";
    }

    try {
        logging::initialize();
    } catch (const error::LogError& e) {
        std::cerr << smyt << e.what() << '\n';
        return 1;
    }

    try {
        logging::log("Starting capture");
    } catch (const error::LogError& e) {
        std::cerr << smyt << e.what() << '\n';
    }

    std::optional<capture::Device> default_device;
    std::optional<std::string> device;

    try {
        default_device = capture::initialize();
    } catch (const error::PcapError& e) {
        std::cerr << smyt << e.what() << '\n';
        goto error_logging;
    }

    device = choose_device(arguments, default_device);

    if (!device) {
        std::cerr << smyt << "No device to capture on\n";
        goto error_logging;
    }

    std::cout << capture::get_library_version() << '\n';
    std::cout << "Capturing on device " << *device << '\n';

    try {
        capture::create_session(*device);
    } catch (const error::PcapError& e) {
        std::cerr << smyt << e.what() << '\n';
        goto error_logging;
    }

    try {
        capture::capture_loop(config, &std::cerr);
    } catch (const error::PcapError& e) {
        std::cerr << smyt << e.what() << '\n';
        goto error_capture;
    }

    try {
        logging::log("Ending capture");
    } catch (const error::LogError& e) {
        std::cerr << smyt << e.what() << '\n';
    }

    capture::destroy_session();
    logging::uninitialize();

    std::cout << std::endl;

    return 0;

error_capture:
    capture::destroy_session();

error_logging:
    logging::uninitialize();

    return 1;
}

int main(int argc, char** argv) {
    args::Arguments arguments;

    try {
        arguments = args::process_arguments(argc, argv);
    } catch (const error::ArgsError& e) {
        std::cerr << smyt << e.what() << '\n';
        args::print_help();
        return 1;
    }

    switch (arguments.action) {
        case args::Action::None:
            assert(false);
            break;
        case args::Action::Capture:
            return capture_main(arguments);
        case args::Action::Help:
            args::print_help();
            break;
        case args::Action::Version:
            args::print_version();
            break;
    }
}
