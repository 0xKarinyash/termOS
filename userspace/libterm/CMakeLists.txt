cmake_minimum_required(VERSION 3.16)
project(libterm LANGUAGES C ASM_NASM OBJC)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OSServices.asm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Console.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/String.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Character.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Process.c
)

add_library(term STATIC ${LIBTERM_SOURCES})

target_compile_options(term PRIVATE $<$<COMPILE_LANGUAGE:C>:${USER_C_FLAGS}>)

target_include_directories(term PUBLIC inc)

add_library(RuntimeEntryObject OBJECT src/RuntimeEntry.asm)
add_library(ObjCRuntimeEntryObject OBJECT src/ObjCRuntimeEntry.asm)

function(add_termos_executable NAME SOURCES)
    set(HAS_OBJC FALSE)
    foreach(SRC ${SOURCES})
        get_filename_component(EXT ${SRC} EXT)
        if(EXT STREQUAL ".m")
            set(HAS_OBJC TRUE)
            break()
        endif()
    endforeach()

    add_executable(${NAME} ${SOURCES})
    
    if(HAS_OBJC)
        target_sources(${NAME} PRIVATE $<TARGET_OBJECTS:ObjCRuntimeEntryObject>)
        target_compile_options(${NAME} PRIVATE $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-runtime=gnustep -fblocks>)
        target_link_options(${NAME} PRIVATE -T ${libterm_SOURCE_DIR}/linker/objc.ld)
        target_link_libraries(${NAME} PRIVATE objc)
    else()
        target_sources(${NAME} PRIVATE $<TARGET_OBJECTS:RuntimeEntryObject>)
    endif()
    
    target_compile_options(${NAME} PRIVATE 
        $<$<OR:$<COMPILE_LANGUAGE:C>,$<COMPILE_LANGUAGE:OBJC>>:${USER_C_FLAGS}>
    )

    target_link_libraries(${NAME} PRIVATE term)
    target_link_options(${NAME} PRIVATE
        -T ${libterm_SOURCE_DIR}/linker/common.ld
        -nostdlib
        -static
    )

    set(ELF2HOT_DIR "${CMAKE_SOURCE_DIR}/tools/elf2hot")
    set(STARTUP_VOLUME_DIR "${CMAKE_BINARY_DIR}/StartupVolume/System/CoreServices")
    file(MAKE_DIRECTORY "${STARTUP_VOLUME_DIR}")
    set(FINAL_HOT_PATH "${STARTUP_VOLUME_DIR}/${NAME}")

    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cargo run --release --quiet -- $<TARGET_FILE:${NAME}> ${FINAL_HOT_PATH}
        WORKING_DIRECTORY ${ELF2HOT_DIR}
        COMMENT "elf2hot: Converting ${NAME} to HOT! format..."
        VERBATIM
    )
endfunction()