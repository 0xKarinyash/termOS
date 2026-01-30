// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <process.h>

extern u64 sys_spawn(const char* path);

u64 spawn(const char* path) {
    return sys_spawn(path);
}