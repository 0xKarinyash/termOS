// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "pmm.h"
#include "bootinfo.h"
#include "panic.h"
#include "types.h"
#include "memory.h" // IWYU pragma: keep // clangd is stupid af
#include "math.h"

extern u64 _kernel_start; 
extern u64 _kernel_end;

u8* bitmap = nullptr;
u64 bitmap_size_g = 0;

void pmm_init(BI_MemoryMap mmap) {
    u64 descriptors_count = mmap.map_size / mmap.descriptor_size;
    u64 max_physical_address = 0;

    for (u64 i = 0; i < descriptors_count; i++) {
        efi_memory_descriptor_k* descriptor  = (efi_memory_descriptor_k*)((u8*)mmap.map + (i * mmap.descriptor_size));
        u64 nominee = descriptor->physical_start + descriptor->number_of_pages * PAGE_SIZE;
        max_physical_address = MAX(nominee, max_physical_address);
    }

    u64 pages_count = max_physical_address / PAGE_SIZE;
    u64 bitmap_size = (pages_count + 7) / 8;
    efi_memory_descriptor_k* desc_to_save = nullptr;

    for (u64 i = 0; i < descriptors_count; i++) {
        efi_memory_descriptor_k* descriptor = (efi_memory_descriptor_k*)((u8*)mmap.map + (i * mmap.descriptor_size));
        // scary
        if ((descriptor->type == EfiConventionalMemory) && \
            ((descriptor->number_of_pages * PAGE_SIZE) >= bitmap_size) && \
            (descriptor->physical_start >= SAFE_SPACE_START_ADDR)) { 
                desc_to_save = descriptor;
                break;
        }
    }

    if (desc_to_save == nullptr) {
        panic("Not enough RAM for bitmap!");
    }

    bitmap = (u8*)desc_to_save->physical_start;
    bitmap_size_g = bitmap_size; 

    memset(bitmap, 0xFF, bitmap_size);

    for (u64 i = 0; i < descriptors_count; i++) {
        efi_memory_descriptor_k* descriptor = (efi_memory_descriptor_k*)((u8*)mmap.map + (i * mmap.descriptor_size));// this shit will haunt my dreams
        if (descriptor->type != EfiConventionalMemory) continue;
        u64 start_addr = descriptor->physical_start;
        u64 end_addr = start_addr + (descriptor->number_of_pages * PAGE_SIZE);
        for (u64 j = start_addr; j < end_addr; j += PAGE_SIZE) BITMAP_UNSET(bitmap, j);
    }
    
    u64 k_start = (u64)&_kernel_start; 
    u64 k_end   = (u64)&_kernel_end;

    u64 bitmap_start = (u64)bitmap;
    u64 bitmap_end   = bitmap_start + bitmap_size_g;

    for (u64 i = 0; i < SAFE_SPACE_START_ADDR; i += PAGE_SIZE) BITMAP_SET(bitmap, i); // mem, that addr < 1MB is NOT safe to use on x86
    for (u64 i = k_start; i < k_end; i += PAGE_SIZE)           BITMAP_SET(bitmap, i); 
    for (u64 i = bitmap_start; i < bitmap_end; i += PAGE_SIZE) BITMAP_SET(bitmap, i);
}

void* pmm_alloc_page() {
    for (u64 i = 0; i < bitmap_size_g; i++) {
        if (bitmap[i] == 0xFF) continue;
        for (u64 j = 0; j < 8; j++) {
            u64 addr = (i * 8 + j) * PAGE_SIZE;
            if ((bitmap[i] & (1 << j)) == 0) { 
                BITMAP_SET(bitmap, addr);
                return (void*)addr;
            }
        }
    }
    return nullptr;
}

void pmm_free_page(void* addr) {
    BITMAP_UNSET(bitmap, (u64)addr);
}
