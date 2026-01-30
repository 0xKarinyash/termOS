cmake_minimum_required(VERSION 3.20)
project(termOSh LANGUAGES C)

file(GLOB_RECURSE TERMOSH_SOURCES "src/*.c")

add_termos_executable(termosh "${TERMOSH_SOURCES}")
target_include_directories(termosh PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/inc"
)