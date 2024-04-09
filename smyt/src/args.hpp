#pragma once

#include <string>

namespace args {
    enum class Action {
        None,
        Capture,
        Help,
        Version
    };

    struct Arguments {
        Action action {Action::None};
        std::string device;
        bool service {false};
    };

    Arguments process_arguments(int argc, char** argv);
    void print_help();
    void print_version();
}
