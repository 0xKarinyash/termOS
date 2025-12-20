cmake_minimum_required(VERSION 3.20)
project(termOS_krn LANGUAGES C ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(
    -ffreestanding
    -mno-red-zone
    -fno-stack-protector
    -fno-builtin
    -Wall -Wextra
)

add_library(shitgui STATIC src/modules/shitgui.c)
target_include_directories(shitgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(kernel 
    src/kmain.c
    src/modules/memory.c
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

set_target_properties(kernel PROPERTIES 
    OUTPUT_NAME "kernel.elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET kernel POST_BUILD
    COMMAND objcopy -O binary $<TARGET_FILE:kernel> $<TARGET_FILE_DIR:kernel>/kernel.bin
    COMMENT "Stripping ELF headers -> kernel.bin"
)