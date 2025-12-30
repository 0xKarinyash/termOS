// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once

#define MASTER_COMMAND 0x20
#define MASTER_DATA    0x21
#define SLAVE_COMMAND  0xA0
#define SLAVE_DATA     0xA1

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile( 
        "outb %0, %1" 
        : 
        : "a"(val), 
        "Nd"(port) );
}

static inline unsigned char inb(unsigned short port) {
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
static inline void io_wait() {
    outb(0x80, 0x0);
}

static inline void outb_wait(unsigned short port, unsigned char val) {
    outb(port, val);
    io_wait();
}
