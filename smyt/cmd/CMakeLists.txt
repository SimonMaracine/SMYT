cmake_minimum_required(VERSION 3.20)

add_executable(smyt
    "src/args.cpp"
    "src/args.hpp"
    "src/main.cpp"
)

target_link_libraries(smyt PRIVATE smyt_common cargs)

target_compile_options(smyt PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")

target_compile_features(smyt PRIVATE cxx_std_20)
set_target_properties(smyt PROPERTIES CXX_EXTENSIONS OFF)
