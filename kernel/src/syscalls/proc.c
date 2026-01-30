// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <syscalls/proc.h>
#include <core/scheduler.h>
#include <drivers/console.h>
#include <core/loader.h>

extern task* curr_task;

u64 sys_exit(i32 code) {
    kprintf("\n[Dewar] process %s exited with code %d", curr_task->proc->name, code);
    sched_exit();
    return code;
}


u64 sys_spawn(const char* path) {
    return process_spawn(path, path);
}

u64 sys_wait(u64 pid) {
    sched_block(pid);
    return pid;
}
