// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

u64 sys_write(u64 fd, u64 buff, u64 len);
u64 sys_read(u64 fd, u64 buff, u64 count);