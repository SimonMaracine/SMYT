#include "args.hpp"

#include <iostream>

#include "error.hpp"

namespace args {
    Arguments process_arguments(int argc, char** argv) {
        if (argc != 2) {
            throw error::ArgsError("Invalid arguments: expected 2, got " + std::to_string(argc));
        }

        Arguments result;
        result.device = argv[1u];

        return result;
    }

    void print_help() {
        std::cout << (
R"(
usage:
    smyt <device>
)"
        );
    }
}
