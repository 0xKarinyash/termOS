// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "bootinfo.h"
#include <core/rand.h>
#include <cpuinfo.h>
#include <types.h>

static u64 prng_state = 0;

static inline u64 rdtsc() {
    u32 lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));

    return ((u64)hi << 32) | lo;
}

void rng_init() {
    prng_state = rdtsc();
    if (prng_state == 0) prng_state = BOOTINFO_MAGIC; // why not reuse "termOS"?
}

static u64 xorshift_rand() {
    u64 x = prng_state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return prng_state = x;
}

static bool hw_rand(u64 *val) {
    u8 ok;
    __asm__ volatile ("rdrand %0; setc %1" 
                    : "=r" (*val), "=qm" (ok));
    return ok != 0;
}

u64 krand() {
    u64 res;
    if (cpu_has(CPU_FEAT_RDRAND)) {
        if (hw_rand(&res)) return res;
    }

    return xorshift_rand();
}