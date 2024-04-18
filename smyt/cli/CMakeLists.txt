cmake_minimum_required(VERSION 3.20)

add_executable(smyt
    "src/args.cpp"
    "src/args.hpp"
    "src/main.cpp"
)

target_link_libraries(smyt PRIVATE smyt_common cargs)

set_compile_options(smyt)
