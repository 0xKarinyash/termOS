cmake_minimum_required(VERSION 3.20)
project(termOS_krn LANGUAGES C ASM_NASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(shitgui STATIC src/modules/shitgui.c)
target_include_directories(shitgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(kernel
    src/entry.asm
    src/asm/interrupts.asm
    src/kmain.c
    src/modules/gdt.c
    src/modules/idt.c
    src/modules/interrupts.c
    src/modules/memory.c
    src/modules/serial.c
    src/modules/console.c
    src/modules/panic.c
    data/font8x8_basic.c
)

target_include_directories(kernel PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include" 
)

target_link_libraries(kernel PRIVATE shitgui)

target_link_options(kernel PRIVATE
    -nostdlib
    -static
    -no-pie
    -T "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

target_compile_options(kernel PRIVATE
    $<$<COMPILE_LANGUAGE:C>:
        -ffreestanding
        -mno-red-zone
        -fno-stack-protector
        -fno-builtin
        -Wall -Wextra
        -g
    >
)

set_target_properties(kernel PROPERTIES 
    OUTPUT_NAME "kernel.elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET kernel POST_BUILD
    COMMAND objcopy -O binary $<TARGET_FILE:kernel> $<TARGET_FILE_DIR:kernel>/kernel.bin
    COMMENT "Stripping ELF headers -> kernel.bin"
)

file(GLOB_RECURSE SOURCES 
    "src/*.c"
    "data/*.c"
)