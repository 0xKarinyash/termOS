// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <core/panic.h>
#include <core/scheduler.h>
#include <mm/heap.h>

task_t* curr_task = nullptr;
u32 next_pid = 1;

extern void irq0_handler();

void sched_init() {
    task_t* kt = (task_t*)malloc(sizeof(task_t));

    kt->id = 0;
    kt->sleep_ticks = 0;
    kt->next = kt;

    curr_task = kt;
}

task_t* sched_spawn(void(*entry)()) {
    task_t* t = (task_t*)malloc(sizeof(task_t));
    if (!t) return nullptr;
    
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
    t->id = next_pid++;
    t->sleep_ticks = 0;
    t->next = curr_task->next;
    curr_task->next = t;

    return t;
}

u64 sched_next(u64 curr_rsp) {
    if (!curr_task) return curr_rsp;

    curr_task->rsp = curr_rsp;
    task_t* it = curr_task->next;

    do {
        if (it->sleep_ticks > 0) it->sleep_ticks--;
        it = it->next;
    } while (it != curr_task->next);

    if (curr_task->sleep_ticks > 0) curr_task->sleep_ticks--;

    task_t* next = curr_task->next;
    while (next != curr_task && next->sleep_ticks > 0) next = next->next; // what the fuck i just wrote  

    curr_task = next;
    return curr_task->rsp;
}

void yield(u64 ticks) {
    curr_task->sleep_ticks = ticks;
    __asm__ volatile("hlt");
}