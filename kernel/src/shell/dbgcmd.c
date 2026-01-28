// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// contents of this file will be changed CONSTANTLY
// im just testing new stuff here

#include "mm/vmm.h"
#include <mm/pmm.h>
#include <shell/dbgcmd.h>
#include <drivers/console.h>

extern u64* pml4_kernel;

#define USER_DS (0x18 | 3)
#define USER_CS (0x20 | 3)
#define RFLAGS_IF 0x202

void jump_to_userspace(void* entry, void* user_stack_top) {
    __asm__ volatile (
        "mov %0, %%ds\n"
        "mov %0, %%es\n"
        "mov %0, %%fs\n"
        "mov %0, %%gs\n"
        :: "r" ((u64)USER_DS) : "memory"
    );

    __asm__ volatile (
        "pushq %0\n" // SS (User Data Selector)
        "pushq %1\n" // RSP (User Stack Pointer)
        "pushq %2\n" // RFLAGS
        "pushq %3\n" // CS (User Code Selector)
        "pushq %4\n" // RIP (Entry Point)
        "iretq\n"    // jump
        : 
        : "r" ((u64)USER_DS),
          "r" ((u64)user_stack_top),
          "r" ((u64)RFLAGS_IF),
          "r" ((u64)USER_CS),
          "r" ((u64)entry)
        : "memory"
    );
}

u64 debug() {
    void* phys_code = pmm_alloc_page();
    void* phys_stack = pmm_alloc_page();
    u64 virt_code = 0x400000;
    u64 virt_stack = 0x800000;

    vmm_map_page(pml4_kernel, (u64)phys_code, virt_code, PTE_PRESENT | PTE_RW | PTE_USER);
    vmm_map_page(pml4_kernel, (u64)phys_stack, virt_stack, PTE_PRESENT | PTE_RW | PTE_USER);
    u8* kernel_view = (u8*)(HHDM_OFFSET + phys_code);
    kernel_view[0] = 0xEB; // jmp
    kernel_view[1] = 0xFE; // to self
    kprintf("Jumping to userspace. Goodbye.");
    jump_to_userspace((void*)virt_code, (void*)(virt_stack + 4096));
    return 0;
}