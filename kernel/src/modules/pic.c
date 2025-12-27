// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "pic.h"
#include "io.h"
#include "types.h"

#define MASTER_COMMAND 0x20
#define MASTER_DATA    0x21
#define SLAVE_COMMAND  0xA0
#define SLAVE_DATA     0xA1

u16 pic_remap() {
    u8 curr_master = inb(MASTER_DATA);
    u8 curr_slave  = inb(SLAVE_DATA);

    // initialization; icw1
    outb_wait(MASTER_COMMAND, 0x11);
    outb_wait(SLAVE_COMMAND, 0x11);

    // icw2
    outb_wait(MASTER_DATA, 0x20); // master now controlling idt[32..39]
    outb_wait(SLAVE_DATA, 0x28); // idt[40..47]

    // icw3
    outb_wait(MASTER_DATA, 0x04); // slave on irq2
    outb_wait(SLAVE_DATA, 0x02); // assign id = 2 to slave

    // icw4; 0x01 -- 8086 mode
    outb_wait(MASTER_DATA, 0x01);
    outb_wait(SLAVE_DATA, 0x01); 

    return ((u16) curr_master << 8) | curr_slave;
}