cmake_minimum_required(VERSION 3.20)

function(set_compile_options TARGET)
    target_compile_options(${TARGET} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")

    target_compile_features(${TARGET} PRIVATE cxx_std_20)
    set_target_properties(${TARGET} PROPERTIES CXX_EXTENSIONS OFF)
endfunction()

add_subdirectory(extern/cargs)
add_subdirectory(extern/json)

add_subdirectory(common)
add_subdirectory(cli)
add_subdirectory(service)
