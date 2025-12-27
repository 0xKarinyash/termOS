// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "panic.h"
#include "types.h"

void isr_handler_c(Registers *regs) {
    panic_exception(regs);
}