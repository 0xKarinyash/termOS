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
    u64 kernel_cr3 = vmm_get_current_cr3();
    for (u64 i = 0; i < header->segments_count; i++) {
        hot_segment* seg = &segments[i];
        if (seg->memsz == 0) continue;

        u64 start = seg->vaddr & ~(0xFFF);
        u64 end = (seg->vaddr + seg->memsz + 0xFFF) & ~(0xFFF);
        for (u64 addr = start; addr < end; addr += PAGE_SIZE) {
            void* phys = pmm_alloc_page();
            vmm_map_page((u64*)proc->pml4_phys, (u64)phys, addr, PTE_USER | PTE_RW | PTE_PRESENT);  
        }

        load_cr3(proc->pml4_phys);
        if (seg->filesz > 0) memcpy((void*)seg->vaddr, data + seg->offset, seg->filesz);
        if (seg->memsz > seg->filesz) {
            u64 bss_start = seg->vaddr + seg->filesz;
            u64 bss_len = seg->memsz - seg->filesz;
            memset((void*)bss_start, 0, bss_len);
        }

        load_cr3(kernel_cr3);
    }

    return header->entry_point;
}