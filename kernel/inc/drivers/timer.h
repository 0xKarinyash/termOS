// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
#ifndef TIMER_H
#define TIMER_H

#include <types.h>

void timer_init(u32 freq);
void timer_handler();
void sleep(u64 ms);
u64 get_uptime();

#endif