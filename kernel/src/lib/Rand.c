// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "bootinfo.h"
#include <lib/Rand.h>
#include <OSCPU.h>
#include <types.h>

static UInt64 sPseudorandState = 0;

static inline UInt64 RDTSC() {
    UInt32 lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));

    return ((UInt64)hi << 32) | lo;
}

void RandInitialize() {
    sPseudorandState = RDTSC();
    if (sPseudorandState == 0) sPseudorandState = BOOTINFO_MAGIC; // why not reuse "termOS"?
}

static UInt64 RandXorshift() {
    UInt64 x = sPseudorandState;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return sPseudorandState = x;
}

static bool RandHardware(UInt64 *value) {
    UInt8 ok;
    __asm__ volatile ("rdrand %0; setc %1" 
                    : "=r" (*value), "=qm" (ok));
    return ok != 0;
}

UInt64 Rand() {
    UInt64 result;
    if (OSCPUHasFeature(kCPUFeatureRDRAND)) {
        if (RandHardware(&result)) return result;
    }

    return RandXorshift();
}