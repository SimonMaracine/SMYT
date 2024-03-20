cmake_minimum_required(VERSION 3.20)

add_executable(smyt
    "src/args.cpp"
    "src/args.hpp"
    "src/capture.cpp"
    "src/capture.hpp"
    "src/error.hpp"
    "src/helpers.cpp"
    "src/helpers.hpp"
    "src/main.cpp"
    "src/notify.cpp"
    "src/notify.hpp"
    "src/packet.cpp"
    "src/packet.hpp"
)

target_include_directories(smyt PUBLIC "src")

if(UNIX)
    target_compile_options(smyt PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(smyt PRIVATE "/W3")
else()
    message(WARNING "Compiler is not MSVC or platform is not supported, warnings are not enabled")
endif()

target_compile_features(smyt PRIVATE cxx_std_20)
set_target_properties(smyt PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(smyt PRIVATE cargs)

# Linux only for now
target_link_libraries(smyt PRIVATE "-lpcap")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNOTIFY REQUIRED "libnotify")

target_include_directories(smyt PRIVATE ${LIBNOTIFY_INCLUDE_DIRS})

target_link_directories(smyt PRIVATE ${LIBNOTIFY_LIBRARY_DIRS})

target_link_libraries(smyt PRIVATE ${LIBNOTIFY_LIBRARIES})
