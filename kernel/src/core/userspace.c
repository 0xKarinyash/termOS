// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <core/userspace.h>
#include <types.h>

#define USER_DS (0x18 | 3)
#define USER_CS (0x20 | 3)
#define RFLAGS_IF 0x202

void jump_to_userspace(void* entry, void* user_stack_top) {
    __asm__ volatile (
        "mov %0, %%ds\n"
        "mov %0, %%es\n"
        "mov %0, %%fs\n"
        :: "r" ((u64)USER_DS) : "memory"
    );

    __asm__ volatile (
        "pushq %0\n" // SS (User Data Selector)
        "pushq %1\n" // RSP (User Stack Pointer)
        "pushq %2\n" // RFLAGS
        "pushq %3\n" // CS (User Code Selector)
        "pushq %4\n" // RIP (Entry Point)
        "iretq\n"
        : 
        : "r" ((u64)USER_DS),
          "r" ((u64)user_stack_top),
          "r" ((u64)RFLAGS_IF),
          "r" ((u64)USER_CS),
          "r" ((u64)entry)
        : "memory"
    );
}