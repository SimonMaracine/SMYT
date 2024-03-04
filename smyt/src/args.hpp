#pragma once

#include <string>

namespace args {
    struct Arugments {
        std::string device;
    };

    Arugments process_arguments(int argc, char** argv);
    void print_help();
}
