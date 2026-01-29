cmake_minimum_required(VERSION 3.20)
project(termOSinit LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

get_filename_component(TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/elf2hot" ABSOLUTE)
set(FINAL_INIT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../initramfs/init")

file(GLOB_RECURSE INIT_SOURCES "src/*.asm" "src/*.c")

add_executable(init_elf ${INIT_SOURCES})

set_target_properties(init_elf PROPERTIES 
    OUTPUT_NAME "init"
    LINKER_LANGUAGE C
)

target_link_options(init_elf PRIVATE
    -nostdlib
    -static
    -T "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

add_custom_command(TARGET init_elf POST_BUILD
    COMMAND cargo run --release --quiet -- $<TARGET_FILE:init_elf> ${FINAL_INIT_PATH}
    
    WORKING_DIRECTORY ${TOOLS_DIR}
    
    COMMENT "Cargo is converting ELF to HOT! format..."
    VERBATIM
)