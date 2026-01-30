// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <core/panic.h>
#include <core/scheduler.h>
#include <core/string.h>
#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/memory.h>
#include <cpuinfo.h>
#include <gdt.h>

task* curr_task = nullptr;
u32 next_pid = 1;

extern void irq0_handler();
extern u64 pml4_kernel_phys;

static process kernel_process;

void idle_task() {
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void sched_init() {
    kernel_process.pid = 0;
    kernel_process.state = RUNNING;
    kernel_process.pml4_phys = pml4_kernel_phys;
    strcpy(kernel_process.name, "kernel");

    task* kt = (task*)malloc(sizeof(task));
    memset(kt, 0, sizeof(task));
    kt->id = 0;
    kt->proc = &kernel_process;
    kt->sleep_ticks = 0;
    kt->next = kt;
    kt->task_state = RUNNING;
    kt->waiting_on_pid = -1;

    curr_task = kt;
    sched_spawn(idle_task, &kernel_process, false, 0);
}

task* sched_spawn(void(*entry)(), process* owner, bool is_user, u64 fixed_user_stack) {
    task* t = (task*)malloc(sizeof(task));
    if (!t) return nullptr;
    if (!owner) owner = &kernel_process;

    u64 stack_size = 16384;
    u8* stack_base = (u8*)malloc(stack_size);
    if (!stack_base) panic("OOM for task stack");
    u64* rsp = (u64*)(stack_base + stack_size); 

    u64 cs = is_user ? 0x23 : 0x08;
    u64 ss = is_user ? 0x1b : 0x10;
    u64 rflags = 0x202;
    u64 target_rsp = 0;
    if (is_user) target_rsp = fixed_user_stack;
    else target_rsp = (u64)stack_base + stack_size;

    *--rsp = ss; // SS -- Kernel data
    *--rsp = target_rsp; // rsp
    *--rsp = rflags; // RFLAGS -- Interrupts Enabled | Reserved bit;
    *--rsp = cs; // CS -- Kernel Code;
    *--rsp = (u64)entry; // RIP

    *--rsp = 0; // int no
    *--rsp = 0; // err code

    for (u8 i = 0; i < 15; i++) *--rsp = 0; // R15 .. RAX

    t->rsp = (u64)rsp;
    t->proc = owner;
    t->id = owner->pid;
    t->sleep_ticks = 0;
    t->next = curr_task->next;
    t->kernel_stack_top = (u64)stack_base + stack_size;
    t->task_state = RUNNING;
    t->waiting_on_pid = -1;
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

    task* next = curr_task;
    while (1) {
        // TODO: add gc here;
        next = next->next;
        if (next->task_state == SLEEPING && next->sleep_ticks == 0) next->task_state = RUNNING;
        if (next->task_state == RUNNING) break;
        if (next == curr_task) {
            if (curr_task->task_state == RUNNING) break;
            panic("no running tasks");
        }
    }

    if (next->proc->pml4_phys != curr_task->proc->pml4_phys) load_cr3(next->proc->pml4_phys);
    curr_task = next;
    tss.rsp0 = curr_task->kernel_stack_top;
    g_cpu.kernel_rsp = curr_task->kernel_stack_top;
    return curr_task->rsp;
}

void sched_block(u32 pid) {
    curr_task->task_state = BLOCKED;
    curr_task->waiting_on_pid = pid;
    __asm__ volatile("int $32");
}

void sched_wakeup(u32 pid) {
    task* it = curr_task;
    do {
        if (it->task_state == BLOCKED && it->waiting_on_pid == (i32)pid) {
            it->task_state = RUNNING;
            it->waiting_on_pid = -1;
        }
        it = it->next;
    } while (it != curr_task);
}

void sched_exit() {
    u32 my_pid = curr_task->id;
    curr_task->task_state = DEAD;

    sched_wakeup(my_pid);
    __asm__ volatile("int $32");
}

void yield(u64 ticks) {
    curr_task->sleep_ticks = ticks;
    curr_task->task_state = SLEEPING;
    __asm__ volatile("hlt");
}