// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>

extern Int32 sys_spawn(const char* path);
extern Int32 sys_wait(UInt64 pid);

Int32 spawn(const char* path) {
    return sys_spawn(path);
}

Int32 wait(UInt64 pid) {
    return sys_wait(pid);
}