cmake_minimum_required(VERSION 3.20)

add_library(smyt_lib STATIC
    "src/capture.cpp"
    "src/capture.hpp"
    "src/configuration.cpp"
    "src/configuration.hpp"
    "src/error.hpp"
    "src/helpers.cpp"
    "src/helpers.hpp"
    "src/logging.cpp"
    "src/logging.hpp"
    "src/packet.cpp"
    "src/packet.hpp"
)

target_include_directories(smyt_lib PUBLIC "src")

target_link_libraries(smyt_lib PRIVATE nlohmann_json "-lpcap")

target_compile_options(smyt_lib PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")

target_compile_features(smyt_lib PRIVATE cxx_std_20)
set_target_properties(smyt_lib PROPERTIES CXX_EXTENSIONS OFF)
