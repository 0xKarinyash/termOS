cmake_minimum_required(VERSION 3.20)
project(termOSdbg LANGUAGES C)

file(GLOB_RECURSE debug_SOURCES "src/*.c")

add_termos_executable(debug "${debug_SOURCES}")
