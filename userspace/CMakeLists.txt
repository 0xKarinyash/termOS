cmake_minimum_required(VERSION 3.20)
project(termOSuserspace LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

message(STATUS "Building termOS's userspace")

add_subdirectory(init)