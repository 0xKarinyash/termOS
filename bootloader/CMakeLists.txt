# Можно не указывать версию, но проект лучше объявить, 
# чтобы IDE (VS Code) понимала, что это отдельный модуль.
project(termOS_Bootloader LANGUAGES C ASM)

# Настройки компиляции ТОЛЬКО для загрузчика
set(UEFI_COMPILE_OPTIONS 
    -std=c23
    -target x86_64-unknown-windows-msvc
    -Wall -Wextra
    $<$<CONFIG:Release>:-Werror>
)

# --- Библиотека POSIX UEFI ---
# Пути указываем относительно текущей папки (bootloader)
set(POSIX_UEFI_SOURCES
    src/uefi/crt_x86_64.c
    src/uefi/dirent.c
    src/uefi/qsort.c
    src/uefi/stat.c
    src/uefi/stdio.c
    src/uefi/stdlib.c
    src/uefi/string.c
    src/uefi/time.c
    src/uefi/unistd.c
)

add_library(posix_uefi_lib STATIC ${POSIX_UEFI_SOURCES})
target_compile_options(posix_uefi_lib PRIVATE ${UEFI_COMPILE_OPTIONS})
# CMAKE_CURRENT_SOURCE_DIR здесь равен .../termOS/bootloader
target_include_directories(posix_uefi_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/uefi)

# --- Сам BOOTX64 ---
add_executable(BOOTX64 src/main.c)
target_compile_options(BOOTX64 PRIVATE ${UEFI_COMPILE_OPTIONS})

target_link_libraries(BOOTX64 PRIVATE posix_uefi_lib)

target_link_options(BOOTX64 PRIVATE
    -fuse-ld=lld
    -target x86_64-unknown-windows-msvc
    -nostdlib
    -Wl,-subsystem:efi_application
    -Wl,-entry:uefi_init
)

set_target_properties(BOOTX64 PROPERTIES 
    SUFFIX ".EFI"
    OUTPUT_NAME "BOOTX64"
    # Это важно! Бинарник положим в корень билда в папку efi_bin,
    # чтобы главный CMakeLists мог его найти
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/efi_bin"
)