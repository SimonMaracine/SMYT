cmake_minimum_required(VERSION 3.20)

project("SMYT" LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")

if(NOT UNIX)
    message(FATAL_ERROR "Platform must be some sort of Linux flavor or at least based on Unix")
endif()

add_subdirectory(smyt)
