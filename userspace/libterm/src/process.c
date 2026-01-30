// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>

extern u64 sys_spawn(const char* path);
extern u64 sys_wait(u64 pid);

u64 spawn(const char* path) {
    return sys_spawn(path);
}

u64 wait(u64 pid) {
    return sys_wait(pid);
}