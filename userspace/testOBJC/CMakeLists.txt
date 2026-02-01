cmake_minimum_required(VERSION 3.20)
project(termOSobjcdbg LANGUAGES OBJC)

file(GLOB_RECURSE testOBJC_SOURCES "src/*.m")

add_termos_executable(testOBJC "${testOBJC_SOURCES}")
