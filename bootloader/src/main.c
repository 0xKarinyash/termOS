// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "uefi.h" // IWYU pragma: keep
#include "../../common/bootinfo.h"

void print(wchar_t* msg) {
    ST->ConOut->OutputString(ST->ConOut, msg);
}

void print_initramfs_warning(wchar_t* reason) {
    print(L"WARNING: Failed to load initramfs file!! Only kernel emergency shell available..\r\nReason: ");
    print(reason);
    print(L"\r\n");
    BS->Stall(2 * 1000 * 1000); // 2s
}

int main()
{
    efi_gop_t* gop = nullptr;
    efi_guid_t gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    ST->BootServices->LocateProtocol(&gop_guid, 0, (void**)&gop);

    efi_loaded_image_protocol_t* loaded_image;
    efi_guid_t lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    gBS->HandleProtocol(IM, &lip_guid, (void**)&loaded_image);

    efi_simple_file_system_protocol_t* fs;
    efi_guid_t sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    gBS->HandleProtocol(loaded_image->DeviceHandle, &sfsp_guid, (void**)&fs);

    efi_file_handle_t* root;
    efi_guid_t fi_guid = EFI_FILE_INFO_GUID;
    fs->OpenVolume(fs, &root);

    // kernel

    efi_file_handle_t* kernel_file;
    uintn_t kinfo_size = 0;
    efi_file_info_t* kfile_info = nullptr;
    efi_status_t status = root->Open(root, &kernel_file, L"kernel.bin", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        print(L"Kernel not found :(");
        while (1);
    }

    status = kernel_file->GetInfo(kernel_file, &fi_guid, &kinfo_size, nullptr);
    
    if (status == EFI_BUFFER_TOO_SMALL) { 
        gBS->AllocatePool(EfiLoaderData, kinfo_size, (void**)&kfile_info);
        status = kernel_file->GetInfo(kernel_file, &fi_guid, &kinfo_size, kfile_info);
    }

    if (EFI_ERROR(status)) {
        print(L"Failed to allocate memory for buffer!!");
        while (1) __asm__("hlt");
    }

    uint64_t kernel_size = kfile_info->FileSize;
    uintn_t kernel_size_read = (uintn_t)kernel_size;
    efi_physical_address_t kernel_addr = 0x100000;
    uintn_t kernel_pages = (kernel_size + 0xFFF) / 0x1000 + 32;

    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, kernel_pages, &kernel_addr);
    if (EFI_ERROR(status)) {
        print(L"Memory 0x100000 busy!\r\n");
        while (1);
    }

    kernel_file->Read(kernel_file, &kernel_size_read, (void*)kernel_addr); // scary!

    // initramfs
    Bootinfo* boot_info = nullptr;
    status = gBS->AllocatePool(EfiLoaderData, sizeof(Bootinfo), (void**)&boot_info);

    boot_info->magic = BOOTINFO_MAGIC;

    efi_file_handle_t* initramfs_file;
    uintn_t iinfo_size = 0;
    efi_file_info_t* ifile_info = nullptr;
    status = root->Open(root, &initramfs_file, L"initramfs.cpio", EFI_FILE_MODE_READ, 0);
    
    if (EFI_ERROR(status)) {
        print_initramfs_warning(L"initramfs.cpio is missing");
        boot_info->initramfs.addr = nullptr;
        boot_info->initramfs.size = 0;
    } else {
        status = initramfs_file->GetInfo(initramfs_file, &fi_guid, &iinfo_size, nullptr);
        if (status == EFI_BUFFER_TOO_SMALL) {
            gBS->AllocatePool(EfiLoaderData, iinfo_size, (void**)&ifile_info);
            status = initramfs_file->GetInfo(initramfs_file, &fi_guid, &iinfo_size, ifile_info);
        }
        if (EFI_ERROR(status)) {
            print_initramfs_warning(L"failed to allocate memory for initramfs buffer!!");
        } else {
            // i hate that nesting 
            uint64_t initramfs_size = ifile_info->FileSize;

            uintn_t initramfs_pages = (initramfs_size + 0xFFF) / 0x1000; 
            efi_physical_address_t initramfs_addr = 0;
            status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData, initramfs_pages, &initramfs_addr);
            if (EFI_ERROR(status)) {
                print_initramfs_warning(L"failed to allocate memory for initramfs itself!!");
            } else {
                // just fucking kill me
                uintn_t initramfs_size_read = (uintn_t)initramfs_size;
                initramfs_file->Read(initramfs_file, &initramfs_size_read, (void*)initramfs_addr);
                boot_info->initramfs.addr = (void*)initramfs_addr;
                boot_info->initramfs.size = initramfs_size;
            }
        }
    }

    boot_info->framebuffer.base = (uint32_t*)gop->Mode->FrameBufferBase;
    boot_info->framebuffer.base_size = gop->Mode->FrameBufferSize;
    boot_info->framebuffer.height = gop->Mode->Information->VerticalResolution;
    boot_info->framebuffer.width = gop->Mode->Information->HorizontalResolution;
    boot_info->framebuffer.pitch = gop->Mode->Information->PixelsPerScanLine;

    uintn_t map_size = 0;
    efi_memory_descriptor_t *map = nullptr;
    uintn_t map_key;
    uintn_t desc_size;
    uint32_t desc_version;

    print(L"Almost ready to jump :D. Working with memory map\r\n");

    gBS->GetMemoryMap(&map_size, nullptr, &map_key, &desc_size, &desc_version);
    map_size += 4096;
    // woah letsgoo
    status = gBS->AllocatePool(EfiLoaderData, map_size, (void**)&map);
    if (EFI_ERROR(status)) {
        print(L"Failed to allocate pool");
    }

    do {
        status = gBS->GetMemoryMap(&map_size, map, &map_key, &desc_size, &desc_version);
        if (EFI_ERROR(status)) {
            break;
        }
        
        boot_info->mem.descriptor_size = desc_size;
        boot_info->mem.descriptor_version = desc_version;
        boot_info->mem.map_size = map_size;
        boot_info->mem.map_key = map_key;
        boot_info->mem.map = map;

        status = gBS->ExitBootServices(IM, map_key);
        if (status == EFI_SUCCESS) {
            break; // FUCK OFF;
        }
        map_size += 2 * desc_size; 
    } while (EFI_ERROR(status));

    typedef void (__attribute__((sysv_abi)) *kentry)(Bootinfo*);
    kentry kmain = (kentry)kernel_addr;

    kmain(boot_info); // yay! :D

    return 0;
}
