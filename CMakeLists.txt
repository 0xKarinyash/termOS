cmake_minimum_required(VERSION 3.20)
project(termOS LANGUAGES C ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(bootloader)
add_subdirectory(kernel)

find_program(MCOPY_EXE mcopy)
find_program(MKFS_EXE mkfs.fat)
find_program(CPIO_EXE cpio)
find_program(QEMU_EXE qemu-system-x86_64)
set(OVMF_PATH "/usr/share/edk2/ovmf/OVMF_CODE.fd" CACHE FILEPATH "Path to OVMF bios")

if(MCOPY_EXE AND MKFS_EXE AND CPIO_EXE)
    set(IMG_FILE "${CMAKE_BINARY_DIR}/termOS.img")
    
    set(INITRAMFS_SRC_DIR "${CMAKE_SOURCE_DIR}/initramfs")
    set(INITRAMFS_CPIO_FILE "${CMAKE_BINARY_DIR}/initramfs.cpio")
    file(GLOB_RECURSE INIT_FILES "${INITRAMFS_SRC_DIR}/*")

    add_custom_command(
        OUTPUT ${INITRAMFS_CPIO_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}
        COMMAND sh -c "find . -mindepth 1 ! -name '*.cpio' -print0 | ${CPIO_EXE} --null -ov -H newc > \"${INITRAMFS_CPIO_FILE}\""
        WORKING_DIRECTORY ${INITRAMFS_SRC_DIR}
        DEPENDS ${INIT_FILES}
        VERBATIM
        COMMENT "Packing initramfs to cpio..."
    )

    add_custom_command(
        OUTPUT ${IMG_FILE}
        COMMAND dd if=/dev/zero of=${IMG_FILE} bs=1M count=64 status=none
        COMMAND ${MKFS_EXE} -F 32 ${IMG_FILE}
        COMMAND mmd -i ${IMG_FILE} ::/EFI ::/EFI/BOOT
        COMMAND ${MCOPY_EXE} -i ${IMG_FILE} $<TARGET_FILE:BOOTX64> ::/EFI/BOOT/BOOTX64.EFI
        COMMAND ${MCOPY_EXE} -i ${IMG_FILE} ${CMAKE_BINARY_DIR}/bin/kernel.bin ::/kernel.bin
        COMMAND ${MCOPY_EXE} -i ${IMG_FILE} ${INITRAMFS_CPIO_FILE} ::/initramfs.cpio
        DEPENDS BOOTX64 kernel ${INITRAMFS_CPIO_FILE}
        VERBATIM
        COMMENT "Generating bootable image: ${IMG_FILE}"
    )

    add_custom_target(image ALL DEPENDS ${IMG_FILE})
endif()

if(QEMU_EXE)
    add_custom_target(run
        COMMAND ${QEMU_EXE}
            -enable-kvm
            -bios ${OVMF_PATH} 
            -drive file=${IMG_FILE},format=raw 
            -vga std
            -net none
            -serial stdio
            -m 512M
        DEPENDS image
    )
endif()