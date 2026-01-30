// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

void timer_init(u32 freq);
u64 timer_handler(Registers *regs);
void sleep(u64 ms);
u64 get_uptime();