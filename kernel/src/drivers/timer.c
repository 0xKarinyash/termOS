// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <drivers/timer.h>
#include <io.h>
#include <types.h>
#include <core/scheduler.h>

#define PIT_BASE_CLOCK 1193180
#define PIT_CMD  0x43
#define PIT_DATA 0x40

volatile u64 ticks = 0;

void timer_init(u32 freq) {
    u32 divisor = PIT_BASE_CLOCK / freq;

    outb(PIT_CMD, 0x36); // 0x36 = 00110110 = channel 0, LOHI byte access, Mode 3, binary
    outb(PIT_DATA, divisor & 0xFF);
    outb(PIT_DATA, (divisor >> 8) & 0xFF);

    u8 mask = inb(0x21); 
    mask &= ~(1 << 0);
    outb(0x21, mask);
}

u64 timer_handler(Registers *regs) {
    ticks++;
    return sched_next((u64)regs);
}


void sleep(u64 ms) {
    u64 start = ticks;
    while (ticks < start + ms) __asm__ volatile ("hlt");
}

u64 get_uptime() {
    return ticks;
}