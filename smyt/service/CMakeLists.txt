cmake_minimum_required(VERSION 3.20)

add_executable(smytd
    "src/daemon.cpp"
    "src/daemon.hpp"
    "src/main.cpp"
)

target_link_libraries(smytd PRIVATE smyt_common "-lsystemd")

target_compile_options(smytd PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")

target_compile_features(smytd PRIVATE cxx_std_20)
set_target_properties(smytd PROPERTIES CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNOTIFY REQUIRED "libsystemd")

target_include_directories(smytd PRIVATE ${LIBSYSTEMD_INCLUDE_DIRS})
target_link_directories(smytd PRIVATE ${LIBSYSTEMD_LIBRARY_DIRS})
target_link_libraries(smytd PRIVATE ${LIBSYSTEMD_LIBRARIES})
