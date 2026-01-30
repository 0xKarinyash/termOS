// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

typedef enum process_state {
    DEAD,
    RUNNING,
    READY,
    BLOCKED,
    SLEEPING,
} process_state;

typedef struct process {
    u64 pid;
    process_state state;
    u64 pml4_phys;
    struct process* parent;
    char name[32];
    u64 heap_start;
    u64 heap_cur;
} process;

typedef struct task {
    u64 rsp;
    struct task* next;
    u32 id;
    u32 sleep_ticks;
    process_state task_state; // reusing process_state cuz wn
    u64 kernel_stack_top;
    process* proc;
    i32 waiting_on_pid;
} task;

void sched_init();
task* sched_spawn(void(*entry)(), process* owner, bool is_user, u64 fixed_user_stack);
u64 sched_next(u64 curr_rsp);
void yield(u64 ticks);
void sched_block(u32 pid);
void sched_wakeup(u32 pid);
void sched_exit(); // suicide