cmake_minimum_required(VERSION 3.16)
project(libobjc LANGUAGES C ASM_NASM OBJC)

set(USER_C_FLAGS
    -ffreestanding
    -fno-builtin
    -nostdlib
    -nostdinc
    -fno-stack-protector
    -fno-pic
    -fno-pie
    -m64
    -mno-red-zone
    -mcmodel=small
    -O2
)

set(LIBOBJC_SOURCES
    src/runtime.c
    src/lookup.c
    src/msgSend.asm
    src/exceptions.c
    src/HOTObject.m
)

add_library(objc STATIC ${LIBOBJC_SOURCES})

target_compile_options(objc PRIVATE $<$<COMPILE_LANGUAGE:C>:${USER_C_FLAGS}>)

target_include_directories(objc PUBLIC inc)
target_link_libraries(objc PRIVATE term)