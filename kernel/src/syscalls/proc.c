// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <syscalls/proc.h>
#include <core/scheduler.h>
#include <core/panic.h>
#include <drivers/console.h>
#include <core/loader.h>

extern task* curr_task;

i32 sys_exit(i32 code) {
    kprintf("\n[Dewar] process %s exited with code %d\n", curr_task->proc->name, code);
    sched_exit();
    return code;
}


i32 sys_spawn(const char* path) {
    return process_spawn(path, path);
}

i32 sys_wait(u64 pid) {
    sched_block(pid);
    return pid;
}
