#include <iostream>
#include <csignal>
#include <optional>
#include <string>

#include "capture.hpp"
#include "error.hpp"
#include "args.hpp"

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

int main(int argc, char** argv) {
    args::Arguments arguments;

    try {
        arguments = args::process_arguments(argc, argv);
    } catch (const error::ArgsError& e) {
        std::cerr << smyt << e.what() << '\n';
        args::print_help();
        return 1;
    }

    if (std::signal(SIGINT, signal_handler) == SIG_ERR) {
        std::cerr << smyt << "Could not setup interrupt handler\n";
        return 1;
    }

    std::optional<capture::Device> default_device;

    try {
        capture::initialize(default_device);
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

    try {
        capture::start_session(*device);

        capture::capture_loop();
    } catch (const error::PcapError& e) {
        std::cerr << smyt << e.what() << '\n';
        return 1;
    }

    capture::stop_session();

    capture::uninitialize();

    std::cout << std::endl;

    return 0;
}
