// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <cpuinfo.h>
#include <syscall.h>
#include <types.h>
#include <drivers/console.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#include <syscalls/proc.h>
#include <syscalls/mem.h>
#include <syscalls/io.h>

static inline void wrmsr(u32 msr, u64 val) {
    u32 low = (u32)val;
    u32 high = (u32)(val >> 32);
    __asm__ volatile("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

static inline u64 rdmsr(u32 msr) {
    u32 low, high;
    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((u64)high << 32) | low;
}

extern void syscall_entry();

void syscall_init() {
    u64 efer = rdmsr(MSR_EFER);
    wrmsr(MSR_EFER, efer | 1); // Enabling SCE in EFER. Just enabling 0 bit

    // setting up STAR
    // 32:47 kernel selector (0x08)
    // 48:64 user selector (0x01 cuz sysret adds +16 for CS and +8 for SS)
    u64 star = (0x13ULL << 48) | (0x08ULL << 32); 
    wrmsr(MSR_STAR, star);

    wrmsr(MSR_LSTAR, (u64)syscall_entry); // setting handler adress

    // masking flags (IA32_FMASK)
    // 9 bit for finterrupts in syscall (IF) and few more necessary flags
    wrmsr(MSR_FMASK, 0x200); // masking only IF

    if (g_cpu.kernel_rsp == 0) {
        void* stack_phys = pmm_alloc_page();
        g_cpu.kernel_rsp = (u64)stack_phys + HHDM_OFFSET + 4096;
    }

    wrmsr(MSR_KERNEL_GS_BASE, (u64)&g_cpu);
}

u64 syscall_dispatch(u64 id, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5) {
    __asm__ volatile("cli");
    switch (id) {
        case SYS_EXIT:  return sys_exit(arg1);
        case SYS_SPAWN: return sys_spawn((const char*)arg1);
        case SYS_MEM:   return sys_mem(arg1);
        case SYS_WRITE: return sys_write(arg1, arg2, arg3);
        case SYS_READ: return sys_read(arg1, arg2, arg3);
        default: kprintf("[Dewar] Unknown syscall %d\n", id); return -1;
    }
    __asm__ volatile("sti");
}