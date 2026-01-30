cmake_minimum_required(VERSION 3.16)
project(libterm LANGUAGES C ASM_NASM)

set(USER_C_FLAGS
    -ffreestanding
    -fno-builtin
    -Wno-incompatible-library-redeclaration # stfu
    -nostdlib
    -nostdinc
    -fno-stack-protector
    -fno-pic
    -fno-pie
    -m64
    -mno-red-zone
    -mcmodel=small
    -Wall
    -Wextra
    -Werror
    -O2
)

set(CMAKE_ASM_NASM_FLAGS "-f elf64")

set(LIBTERM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stdio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/malloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ctype.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/process.c
)

add_library(term STATIC ${LIBTERM_SOURCES})

target_compile_options(term PRIVATE $<$<COMPILE_LANGUAGE:C>:${USER_C_FLAGS}>)

target_include_directories(term PUBLIC inc)

add_library(crt0_obj OBJECT src/crt0.asm)
target_compile_options(crt0_obj PRIVATE $<$<COMPILE_LANGUAGE:C>:${USER_C_FLAGS}>)

function(add_termos_executable NAME SOURCES)
    add_executable(${NAME} ${SOURCES})
    
    target_sources(${NAME} PRIVATE $<TARGET_OBJECTS:crt0_obj>)
    
    target_compile_options(${NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:${USER_C_FLAGS}>)
    target_link_libraries(${NAME} PRIVATE term)
    
    target_link_options(${NAME} PRIVATE
        -T ${libterm_SOURCE_DIR}/linker.ld
        -nostdlib
        -static
    )

    set(ELF2HOT_DIR "${CMAKE_SOURCE_DIR}/tools/elf2hot")
    set(FINAL_HOT_PATH "${CMAKE_SOURCE_DIR}/initramfs/bin/${NAME}")

    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cargo run --release --quiet -- $<TARGET_FILE:${NAME}> ${FINAL_HOT_PATH}
        WORKING_DIRECTORY ${ELF2HOT_DIR}
        COMMENT "elf2hot: Converting ${NAME} to HOT! format..."
        VERBATIM
    )
endfunction()