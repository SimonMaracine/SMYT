#include <iostream>
#include <csignal>
#include <optional>
#include <string>
#include <cassert>

#include "capture.hpp"
#include "error.hpp"
#include "args.hpp"
#include "notify.hpp"

static const char* smyt {"smyt: "};

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

    try {
        notify::initialize();
    } catch (const error::LibnotifyError& e) {
        std::cerr << smyt << e.what() << '\n';
        return 1;
    }

    try {
        notify::notify("Starting Capture", std::nullopt);
    } catch (const error::LibnotifyError& e) {
        std::cerr << smyt << e.what() << '\n';
    }

    std::optional<capture::Device> default_device;

    try {
        default_device = capture::initialize();
    } catch (const error::PcapError& e) {
        std::cerr << smyt << e.what() << '\n';
        return 1;
    }

    const auto device {choose_device(arguments, default_device)};

    if (!device) {
        std::cerr << smyt << "No device to capture on\n";
        return 1;
    }

    std::cout << capture::get_library_version() << '\n';
    std::cout << "Capturing on device " << *device << '\n';

    try {
        capture::start_session(*device);
        capture::capture_loop();
    } catch (const error::PcapError& e) {
        std::cerr << smyt << e.what() << '\n';
        return 1;
    }

    capture::stop_session();

    capture::uninitialize();

    notify::uninitialize();

    std::cout << std::endl;

    return 0;
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
