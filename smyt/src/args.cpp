#include "args.hpp"

#include <iostream>

#include "error.hpp"

namespace args {
    Arguments process_arguments(int argc, char** argv) {
        Arguments result;

        if (argc > 1) {
            result.device = argv[1u];
        }

        // TODO remove
        // throw error::ArgsError("Invalid arguments: expected 2, got " + std::to_string(argc));

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
