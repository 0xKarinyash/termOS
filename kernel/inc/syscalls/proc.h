// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>

i32 sys_exit(i32 code);
i32 sys_spawn(const char* path);
i32 sys_wait(u64 pid);