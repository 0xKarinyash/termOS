#include "uefi.h" // IWYU pragma: keep
#include "../../common/bootinfo.h"


int main()
{
    efi_gop_t *gop = nullptr;
    efi_guid_t gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    ST->BootServices->LocateProtocol(&gop_guid, 0, (void**)&gop);

    efi_loaded_image_protocol_t *loaded_image;
    efi_guid_t lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    gBS->HandleProtocol(IM, &lip_guid, (void**)&loaded_image);

    efi_simple_file_system_protocol_t *fs;
    efi_guid_t sfsp_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    gBS->HandleProtocol(loaded_image->DeviceHandle, &sfsp_guid, (void**)&fs);

    efi_file_handle_t *root;
    fs->OpenVolume(fs, &root);

    efi_file_handle_t *kernel_file;
    efi_guid_t fi_guid = EFI_FILE_INFO_GUID;
    uintn_t info_size = 0;
    efi_file_info_t *file_info = nullptr;
    efi_status_t status = root->Open(root, &kernel_file, L"kernel.bin", EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Kernel not found :(");
        while (1);
    }

    status = kernel_file->GetInfo(kernel_file, &fi_guid, &info_size, NULL);
    
    if (status == EFI_BUFFER_TOO_SMALL) { 
        gBS->AllocatePool(EfiLoaderData, info_size, (void**)&file_info);
        status = kernel_file->GetInfo(kernel_file, &fi_guid, &info_size, file_info);
    }

    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Failed to allocate memory for buffer!!");
        while (1) __asm__("hlt");
    }

    uint64_t kernel_size = file_info->FileSize;
    uintn_t kernel_size_read = (uintn_t)kernel_size;
    efi_physical_address_t kernel_addr = 0x100000;
    uintn_t pages = (kernel_size + 0xFFF) / 0x1000;

    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, pages, &kernel_addr);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Memory 0x100000 busy!\r\n");
        while (1);
    }

    kernel_file->Read(kernel_file, &kernel_size_read, (void*)kernel_addr); // scary!

    Bootinfo *boot_info = NULL;

    status = gBS->AllocatePool(EfiLoaderData, sizeof(Bootinfo), (void**)&boot_info);

    boot_info->framebuffer.base = (uintn_t*)gop->Mode->FrameBufferBase;
    boot_info->framebuffer.base_size = gop->Mode->FrameBufferSize;
    boot_info->framebuffer.height = gop->Mode->Information->VerticalResolution;
    boot_info->framebuffer.width = gop->Mode->Information->HorizontalResolution;
    boot_info->framebuffer.pitch = gop->Mode->Information->PixelsPerScanLine;

    uintn_t map_size = 0;
    efi_memory_descriptor_t *map = NULL;
    uintn_t map_key;
    uintn_t desc_size;
    uint32_t desc_version;

    ST->ConOut->OutputString(ST->ConOut, L"Almost ready to jump :D. Working with memory map\r\n");

    gBS->GetMemoryMap(&map_size, NULL, &map_key, &desc_size, &desc_version);
    map_size += 4096;
    // woah letsgoo
    status = gBS->AllocatePool(EfiLoaderData, map_size, (void**)&map);
    if (EFI_ERROR(status)) {
        ST->ConOut->OutputString(ST->ConOut, L"Failed to allocate pool");
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
