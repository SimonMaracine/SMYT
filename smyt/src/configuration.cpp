#include "configuration.hpp"

#include <fstream>
#include <filesystem>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "error.hpp"

#define CONFIGURATION_FILE_DIRECTORY "/etc/smyt"
#define CONFIGURATION_FILE_PATH (CONFIGURATION_FILE_DIRECTORY "/conf.json")

namespace configuration {
    static void create_configuration_file() {
        std::ofstream stream {CONFIGURATION_FILE_PATH};

        if (!stream.is_open()) {
            throw error::ConfigError("Could not create configuration file");
        }

        Config default_config;

        nlohmann::json root;
        root["process_period"] = default_config.process_period;
        root["warning_threshold"] = default_config.warning_threshold;
        root["panic_threshold"] = default_config.panic_threshold;

        stream << std::setw(4) << root;

        if (stream.bad()) {
            throw error::ConfigError("Unexpected error writing to configuration file");
        }
    }

    static Config load_configuration() {
        if (!std::filesystem::exists(CONFIGURATION_FILE_DIRECTORY)) {
            std::error_code err;

            if (!std::filesystem::create_directories(CONFIGURATION_FILE_DIRECTORY, err)) {
                throw error::ConfigError("Could not create configuration directory: " + err.message());
            }

            create_configuration_file();
        }

        std::ifstream stream {CONFIGURATION_FILE_PATH};

        Config config;

        nlohmann::json root;

        try {
            stream >> root;
        } catch (const nlohmann::json::exception&) {
            try { create_configuration_file(); } catch (...) {}

            throw error::ConfigError("Error reading from JSON configuration file");
        }

        if (stream.bad()) {
            throw error::ConfigError("Unexpected error reading from configuration file");
        }

        try {
            config.process_period = root["process_period"].get<long>();
            config.warning_threshold = root["warning_threshold"].get<std::size_t>();
            config.panic_threshold = root["panic_threshold"].get<std::size_t>();
        } catch (const nlohmann::json::exception&) {
            throw error::ConfigError("Error reading from JSON configuration file");
        }

        return config;
    }

    void load(Config& config) {
        config = load_configuration();
    }
}
