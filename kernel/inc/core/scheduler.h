// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>
typedef struct task {
    u64 rsp;
    struct task* next;
    u32 id;
    u32 sleep_ticks;
} task_t;

void sched_init();
task_t* sched_spawn(void(*entry)());
u64 sched_next(u64 curr_rsp);
void yield(u64 ticks);