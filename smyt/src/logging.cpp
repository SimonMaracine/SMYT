#include "logging.hpp"

#include <fstream>
#include <ctime>
#include <filesystem>

#include "error.hpp"

namespace logging {
    #define LOG_FILE_DIRECTORY "/var/log/smyt"
    #define LOG_FILE_PATH (LOG_FILE_DIRECTORY "/smyt.log")

    static std::ofstream stream;

    static std::string get_current_time() {
        const std::time_t current_time {std::time(nullptr)};

        std::string result {std::asctime(std::localtime(&current_time))};

        return result.erase(result.size() - 1u);
    }

    void initialize() {
        if (!std::filesystem::exists(LOG_FILE_DIRECTORY)) {
            std::error_code err;

            if (!std::filesystem::create_directories(LOG_FILE_DIRECTORY), err) {
                throw error::LogError("Could not create log directory: " + err.message());
            }
        }

        stream.open(LOG_FILE_PATH, std::ios_base::app);

        if (!stream.is_open()) {
            throw error::LogError("Could not open log file");
        }
    }

    void uninitialize() {
        stream.close();
    }

    void log(const std::string& message, bool flush) {
        stream << '[' << get_current_time() << "] " << message << '\n';

        if (stream.bad()) {
            stream.clear();
            throw error::LogError("Unexpected error writing to log file");
        }

        if (flush) {
            stream.flush();
        }
    }
}
