// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// contents of this file will be changed CONSTANTLY
// im just testing new stuff here

#include <drivers/timer.h>
#include <drivers/console.h>
#include <types.h>
#include <shell/dbgcmd.h>

u64 debug() {
    kprintf("Float test\n");
    kprintf("Trying 3.14 + 1.44 in 3 seconds..\n");
    sleep(3000);
    volatile float a = 3.14;
    volatile float b = 1.44;
    volatile float c = a + b;
    kprintf("%d", (i32)c);
    return 0;
}