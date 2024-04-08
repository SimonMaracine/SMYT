cmake_minimum_required(VERSION 3.20)

add_executable(smyt
    "src/args.cpp"
    "src/args.hpp"
    "src/capture.cpp"
    "src/capture.hpp"
    "src/configuration.cpp"
    "src/configuration.hpp"
    "src/error.hpp"
    "src/helpers.cpp"
    "src/helpers.hpp"
    "src/logging.cpp"
    "src/logging.hpp"
    "src/main.cpp"
    "src/packet.cpp"
    "src/packet.hpp"
)

target_include_directories(smyt PUBLIC "src")

target_compile_options(smyt PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")

target_compile_features(smyt PRIVATE cxx_std_20)
set_target_properties(smyt PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(smyt PRIVATE cargs "-lpcap" nlohmann_json)
