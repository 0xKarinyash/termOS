cmake_minimum_required(VERSION 3.20)
project(termOS_krn LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT DEFINED ARCH)
    set(ARCH "x86_64")
endif()

message(STATUS "TermOS Kernel: Building for architecture '${ARCH}'")


file(GLOB_RECURSE KERNEL_SOURCES CMAKE_CONFIGURE_DEPENDS
    "src/kmain.c"
    
    "src/arch/${ARCH}/*.c"
    "src/arch/${ARCH}/*.asm"

    "src/core/*.c"
    "src/drivers/*.c"
    "src/mm/*.c"
    "src/shell/*.c"
    "src/fs/*.c"
    
    "data/*.c"
)

add_executable(kernel ${KERNEL_SOURCES})

target_include_directories(kernel PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/arch/${ARCH}"
    "${CMAKE_CURRENT_SOURCE_DIR}/data"
)

target_compile_options(kernel PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -ffreestanding
        -mno-red-zone
        -fno-stack-protector
        -fno-builtin
        -Wall -Wextra
        -mcmodel=kernel
        -g
    >
)

target_link_options(kernel PRIVATE
    -nostdlib
    -static
    -no-pie
    -T "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
    -z max-page-size=0x1000 
)

set_target_properties(kernel PROPERTIES 
    OUTPUT_NAME "kernel.elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET kernel POST_BUILD
    COMMAND objcopy -O binary $<TARGET_FILE:kernel> $<TARGET_FILE_DIR:kernel>/kernel.bin
    COMMENT "Generating raw binary kernel.bin..."
)