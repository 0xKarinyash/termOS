cmake_minimum_required(VERSION 3.20)
project(termOSinit LANGUAGES C)

file(GLOB_RECURSE INIT_SOURCES "src/*.c")

add_termos_executable(init "${INIT_SOURCES}")
