// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>

enum {
    kIOMasterCommand = 0x20,
    kIOMasterData    = 0x21,
    kIOSlaveCommand  = 0xA0,
    kIOSlaveData     = 0xA1,
    kIOPortWaitAddress = 0x80
};

static inline void IOPortWrite8(UInt16 port, UInt8 val) {
    __asm__ volatile( 
        "outb %0, %1" 
        : 
        : "a"(val), 
        "Nd"(port) );
}

static inline unsigned char IOPortRead8(UInt16 port) {
    unsigned char ret;
    __asm__ volatile(
        "inb %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );

    return ret;
}

// sending junk to 0x80 (unused) to skip few tacts
// not ideal i know
static inline void IOSynchronizeBus() {
    IOPortWrite8(kIOPortWaitAddress, 0x0);
}

static inline void IOPortWrite8WithWait(UInt16 port, UInt8 val) {
    IOPortWrite8(port, val);
    IOSynchronizeBus();
}
