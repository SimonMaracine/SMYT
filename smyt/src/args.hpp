#pragma once

#include <string>

namespace args {
    struct Arguments {
        std::string device;
    };

    Arguments process_arguments(int argc, char** argv);
    void print_help();
}
