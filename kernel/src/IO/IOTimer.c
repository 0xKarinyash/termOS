// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <IO/IOTimer.h>
#include <IO.h>
#include <types.h>
#include <OS/OSScheduler.h>

enum {
    kIOTimerPitBaseClock = 1193180,
    kIOTimerPitCommand = 0x43,
    kIOTimerPitData = 0x40,
};

static volatile UInt64 sIOTimerTicks = 0;

void IOTimerInitialize(UInt32 frequency) {
    UInt32 divisor = kIOTimerPitBaseClock / frequency;

    IOPortWrite8(kIOTimerPitCommand, 0x36); // 0x36 = 00110110 = channel 0, LOHI byte access, Mode 3, binary
    IOPortWrite8(kIOTimerPitData, divisor & 0xFF);
    IOPortWrite8(kIOTimerPitData, (divisor >> 8) & 0xFF);

    UInt8 mask = IOPortRead8(0x21); 
    mask &= ~(1 << 0);
    IOPortWrite8(0x21, mask);
}

UInt64 IOTimerInterruptsHandler(CPURegisters *registers) {
    sIOTimerTicks++;
    return OSSchedulerNext((UInt64)registers);
}

UInt64 IOTimerGetTicks() {
    return sIOTimerTicks;
}