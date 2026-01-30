// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>

u64 sys_exit(i32 code);
u64 sys_spawn(const char* path);