// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "drivers/timer.h"
#include <io.h>
#include <core/panic.h>
#include <drivers/keyboard.h>
#include <shell/builtins.h>
#include <types.h>


void isr_handler_c(Registers *regs) {
    if (regs->int_no == 3) {
        return print_regs();
    }
    panic_exception(regs);
}

void irq_handler_c(Registers *regs) {
    switch (regs->int_no) {
        case 32: return timer_handler();
        case 33: return kb_handler();
        default: outb(SLAVE_COMMAND, 0x20); break;
    }

    outb(MASTER_COMMAND, 0x20);
}