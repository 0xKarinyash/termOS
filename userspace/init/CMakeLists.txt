cmake_minimum_required(VERSION 3.20)
project(termOSinit LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

message(STATUS "Building termOS's init")

file(GLOB_RECURSE INIT_SOURCES CMAKE_CONFIGURE_DEPENDS
    "src/*.asm"
    "src/*.c"
)

add_executable(init ${INIT_SOURCES})
set_target_properties(init PROPERTIES
    SUFFIX ""
    LINKER_LANGUAGE C 
)

target_link_options(init PRIVATE
    -nostdlib
    -static
    -Wl,--oformat=binary
    -Wl,-Ttext=0x400000
    -Wl,-e,start
)

add_custom_command(TARGET init POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:init> ${CMAKE_SOURCE_DIR}/initramfs/init
    COMMENT "Installing init binary to initramfs"
)