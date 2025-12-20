cmake_minimum_required(VERSION 3.20)
project(termOS_krn LANGUAGES C ASM)

# === 1. Настройки стандарта (C23 для флекса) ===
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# === 2. Общие флаги компиляции ===
# Они применятся И к ядру, И к shitgui автоматически.
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

# Подключаем папки заголовков
target_include_directories(kernel PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"  # Для bootinfo.h
    "${CMAKE_CURRENT_SOURCE_DIR}/include"    # Для types.h и shitgui.h
)

# Линкуем shitgui к ядру
target_link_libraries(kernel PRIVATE shitgui)

# === 5. Линковка (Linker) ===
target_link_options(kernel PRIVATE
    -nostdlib
    -static
    -no-pie
    -T "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

# === 6. Выходные файлы ===
set_target_properties(kernel PROPERTIES 
    OUTPUT_NAME "kernel.elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Превращаем ELF в BIN (Flat Binary)
add_custom_command(TARGET kernel POST_BUILD
    COMMAND objcopy -O binary $<TARGET_FILE:kernel> $<TARGET_FILE_DIR:kernel>/kernel.bin
    COMMENT "Stripping ELF headers -> kernel.bin"
)