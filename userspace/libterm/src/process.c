// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>

extern i32 sys_spawn(const char* path);
extern i32 sys_wait(u64 pid);

i32 spawn(const char* path) {
    return sys_spawn(path);
}

i32 wait(u64 pid) {
    return sys_wait(pid);
}