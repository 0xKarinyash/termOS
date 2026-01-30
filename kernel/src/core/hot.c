// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <core/hot.h>
#include <core/scheduler.h>
#include <core/string.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <mm/memory.h>

#include "../../common/hot_header.h"

u64 load_hot(process* proc, u8* data) {
    hot_header* header = (hot_header*)data;
    if (header->magic != HOT_MAGIC) {
        return -1;
    }

    hot_segment* segments = (hot_segment*)(data + sizeof(hot_header));
    for (u64 i = 0; i < header->segments_count; i++) {
        hot_segment* seg = &segments[i];
        if (seg->memsz == 0) continue;

        u64 start = seg->vaddr & ~(0xFFFULL);
        u64 end = (seg->vaddr + seg->memsz + 0xFFF) & ~(0xFFFULL);

        for (u64 addr = start; addr < end; addr += PAGE_SIZE) {
            void* phys = pmm_alloc_page();
            vmm_map_page((u64*)proc->pml4_phys, (u64)phys, addr, PTE_USER | PTE_RW | PTE_PRESENT);
            void* kernel_virt = (void*)((u64)phys + HHDM_OFFSET);
            memset(kernel_virt, 0, PAGE_SIZE);
            u64 page_overleap_start = (addr > seg->vaddr) ? addr : seg->vaddr;
            u64 page_overleap_end = (addr + PAGE_SIZE < seg->vaddr + seg->filesz) 
                ? (addr + PAGE_SIZE) 
                : (seg->vaddr + seg->filesz);
            if (page_overleap_start < page_overleap_end) {
                u64 copy_size = page_overleap_end - page_overleap_start;
                u64 src_offset = seg->offset + (page_overleap_start - seg->vaddr);
                u64 dst_offset = page_overleap_start - addr;

                memcpy((u8*)kernel_virt + dst_offset, data + src_offset, copy_size);
            }
        }
    }

    return header->entry_point;
}