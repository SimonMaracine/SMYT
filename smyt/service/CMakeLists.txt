cmake_minimum_required(VERSION 3.20)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNOTIFY REQUIRED "libsystemd")

target_include_directories(smyt_lib PRIVATE ${LIBSYSTEMD_INCLUDE_DIRS})
target_link_directories(smyt_lib PRIVATE ${LIBSYSTEMD_LIBRARY_DIRS})
target_link_libraries(smyt_lib PRIVATE ${LIBSYSTEMD_LIBRARIES})

add_executable(smytd
    "src/daemon.cpp"
    "src/daemon.hpp"
    "src/main.cpp"
)

target_link_libraries(smytd PRIVATE smyt_lib "-lsystemd")

target_compile_options(smytd PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")

target_compile_features(smytd PRIVATE cxx_std_20)
set_target_properties(smytd PROPERTIES CXX_EXTENSIONS OFF)
