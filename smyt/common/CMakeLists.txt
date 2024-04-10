cmake_minimum_required(VERSION 3.20)

add_library(smyt_common STATIC
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

target_include_directories(smyt_common PUBLIC "src")

target_link_libraries(smyt_common PRIVATE nlohmann_json "-lpcap")

set_compile_options(smyt_common)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(smyt_common PUBLIC "NDEBUG")
endif()
