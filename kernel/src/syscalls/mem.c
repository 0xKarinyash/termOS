// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <syscalls/mem.h>

#include <core/scheduler.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/memory.h>

extern task* curr_task;

u64 sys_mem(u64 size) {
    if (size == 0) return 0;
    process* proc = curr_task->proc;
    u64 addr_to_ret = proc->heap_cur;

    u64 pages_needed = (size + (PAGE_SIZE-1)) / PAGE_SIZE;

    for (u64 i = 0; i < pages_needed; i++) {
        void* phys = pmm_alloc_page();
        if (!phys) return 0;

        vmm_map_page((u64*)proc->pml4_phys, (u64)phys, proc->heap_cur, PTE_PRESENT | PTE_RW | PTE_USER);
        memset((void*)PHYS_TO_HHDM((u64)phys), 0, PAGE_SIZE);

        proc->heap_cur += 4096;
    }

    return addr_to_ret;
}
