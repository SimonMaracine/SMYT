cmake_minimum_required(VERSION 3.20)

add_subdirectory(extern/cargs)
add_subdirectory(extern/json)

add_subdirectory(lib)
add_subdirectory(command_line)
add_subdirectory(service)
