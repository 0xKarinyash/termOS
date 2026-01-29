// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <core/panic.h>
#include <core/scheduler.h>
#include <core/string.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <cpuinfo.h>
#include <gdt.h>

task* curr_task = nullptr;
u32 next_pid = 1;


extern void irq0_handler();
extern u64 pml4_kernel_phys;

static process kernel_process;

void sched_init() {
    kernel_process.pid = 0;
    kernel_process.state = RUNNING;
    kernel_process.pml4_phys = pml4_kernel_phys;
    strcpy(kernel_process.name, "kernel");

    task* kt = (task*)malloc(sizeof(task));
    kt->id = 0;
    kt->proc = &kernel_process;
    kt->sleep_ticks = 0;
    kt->next = kt;

    curr_task = kt;
}

task* sched_spawn(void(*entry)(), process* owner) {
    task* t = (task*)malloc(sizeof(task));
    if (!t) return nullptr;
    if (!owner) owner = &kernel_process;

    u64 stack_size = 16384;
    u8* stack_base = (u8*)malloc(stack_size);
    if (!stack_base) panic("OOM for task stack");
    u64* rsp = (u64*)(stack_base + stack_size); 

    *--rsp = 0x10; // SS -- Kernel data
    *--rsp = (u64)stack_base + stack_size; // rsp
    *--rsp = 0x202; // RFLAGS -- Interrupts Enabled | Reserved bit;
    *--rsp = 0x08; // CS -- Kernel Code;
    *--rsp = (u64)entry; // RIP

    *--rsp = 0; // int no
    *--rsp = 0; // err code

    for (u8 i = 0; i < 15; i++) *--rsp = 0; // R15 .. RAX

    t->rsp = (u64)rsp;
    t->proc = owner;
    t->id = next_pid++;
    t->sleep_ticks = 0;
    t->next = curr_task->next;
    t->kernel_stack_top = (u64)stack_base + stack_size;
    curr_task->next = t;
    return t;
}

u64 sched_next(u64 curr_rsp) {
    if (!curr_task) return curr_rsp;

    curr_task->rsp = curr_rsp;
    task* it = curr_task->next;

    do {
        if (it->sleep_ticks > 0) it->sleep_ticks--;
        it = it->next;
    } while (it != curr_task->next);

    if (curr_task->sleep_ticks > 0) curr_task->sleep_ticks--;

    task* next = curr_task->next;
    while (next != curr_task && next->sleep_ticks > 0) next = next->next; // what the fuck i just wrote  

    if (next->proc->pml4_phys != curr_task->proc->pml4_phys) load_cr3(next->proc->pml4_phys);

    curr_task = next;
    tss.rsp0 = curr_task->kernel_stack_top;
    g_cpu.kernel_rsp = curr_task->kernel_stack_top;
    return curr_task->rsp;
}

void yield(u64 ticks) {
    curr_task->sleep_ticks = ticks;
    __asm__ volatile("hlt");
}