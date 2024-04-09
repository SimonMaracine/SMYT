#include "args.hpp"

#include <iostream>
#include <cstdio>

#include <cargs.h>

#include "error.hpp"

namespace args {
    static struct cag_option OPTIONS[] {
        {
            .identifier = 'i',
            .access_letters = "i",
            .access_name = "interface",
            .value_name = "INTERFACE",
            .description = "Capture on a specific interface"
        },
        {
            .identifier = 's',
            .access_letters = nullptr,
            .access_name = "service",
            .value_name = nullptr,
            .description = "Run as a systemd service. DO NOT USE THIS OPTION."
        },
        {
            .identifier = 'h',
            .access_letters = "h",
            .access_name = "help",
            .value_name = nullptr,
            .description = "Show this help message"
        },
        {
            .identifier = 'v',
            .access_letters = "v",
            .access_name = "version",
            .value_name = nullptr,
            .description = "Show the version of this program"
        }
    };

    Arguments process_arguments(int argc, char** argv) {
        Arguments result;

        cag_option_context context;
        cag_option_init(&context, OPTIONS, CAG_ARRAY_SIZE(OPTIONS), argc, argv);

        while (cag_option_fetch(&context)) {
            switch (cag_option_get_identifier(&context)) {
                case 'i': {
                    result.action = Action::Capture;

                    const char* value {cag_option_get_value(&context)};

                    if (value == nullptr) {
                        throw error::ArgsError("Expected argument for `-i`");
                    }

                    result.device = value;

                    break;
                }
                case 's':
                    result.service = true;

                    break;
                case 'h':
                    if (result.action == Action::None) {
                        result.action = Action::Help;
                    }

                    break;
                case 'v':
                    if (result.action == Action::None) {
                        result.action = Action::Version;
                    }

                    break;
                case '?':
                    char message[64u];
                    cag_option_print_error_string(&context, message, 64u);

                    throw error::ArgsError(message);
            }
        }

        // Default is capturing
        if (result.action == Action::None) {
            result.action = Action::Capture;
        }

        return result;
    }

    void print_help() {
        std::cout << (
            "usage:\n"
            "    smyt -i <interface>\n"
            "    smyt -h\n"
            "    smyt -v\n\n"
        );

        cag_option_print(OPTIONS, CAG_ARRAY_SIZE(OPTIONS), stdout);
    }

    void print_version() {
        static constexpr unsigned int V_MAJOR {0u};
        static constexpr unsigned int V_MINOR {1u};
        static constexpr unsigned int V_PATCH {0u};

        std::cout << "smyt version " << V_MAJOR << '.' << V_MINOR << '.' << V_PATCH << '\n';
    }
}
