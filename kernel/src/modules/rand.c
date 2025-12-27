// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "rand.h"
#include "types.h"

// really shiity rand lol. 
// not shitty.. basic.

u64 shitrand() {
    int lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));

    return ((u64)hi << 32) | lo;
}