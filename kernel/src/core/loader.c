// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <core/loader.h>
#include <core/userspace.h>
#include <core/scheduler.h>

#include <shell/ksh.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/memory.h>

#include <fs/vfs.h>

#include <drivers/console.h>
#include <types.h>

extern u64* pml4_kernel;

bool exec_init(const char* path) {
    kprintf("[Loader] loading %s...\n", path);
    fs_node* file = vfs_open(path);
    if (!file) {
        kprintf("[Loader] Error: %s not found in initramfs!\n", path);
        return false;
    }

    u64 virt_code = 0x400000;
    u64 virt_stack = 0x800000;
    u64 stack_size = 8192;     

    u64 bytes_left = file->len;
    u64 offset = 0;
    u64 page_idx = 0;

    while (bytes_left > 0) {
        void* phys = pmm_alloc_page();
        if (!phys) {
            kprintf("Loader: OOM!\n");
            return false;
        }

        vmm_map_page(pml4_kernel, (u64)phys, virt_code + (page_idx * 4096), PTE_PRESENT | PTE_RW | PTE_USER);

        void* k_ptr = (void*)(HHDM_OFFSET + (u64)phys);
        memset(k_ptr, 0, 4096);
        
        u64 chunk = (bytes_left > 4096) ? 4096 : bytes_left;
        vfs_read(file, offset, chunk, (u8*)k_ptr);

        bytes_left -= chunk;
        offset += chunk;
        page_idx++;
    }

    for (u64 i = 0; i < (stack_size / 4096); i++) {
        void* phys = pmm_alloc_page();
        vmm_map_page(pml4_kernel, (u64)phys, virt_stack + (i * 4096), PTE_PRESENT | PTE_RW | PTE_USER);
        memset((void*)(HHDM_OFFSET + (u64)phys), 0, 4096);
    }

    __asm__ volatile(
        "mov %%cr3, %%rax\n\t"
        "mov %%rax, %%cr3\n\t"
        ::: "rax", "memory"
    );

    kprintf("[Loader] Transferring control to userspace...\n");
    jump_to_userspace((void*)virt_code, (void*)(virt_stack + stack_size));
    
    return true; // unreachable
}

void init_task_entry() {
    if (!exec_init("/init")) {
        kprintf("FATAL: Could not load /init\n");
        sched_spawn(ksh); 
        
        while(1) __asm__("hlt"); 
    }
}