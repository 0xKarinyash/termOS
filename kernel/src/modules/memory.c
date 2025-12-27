// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "memory.h"
#include "types.h"

void *memset(void *ptr, int value, usize num) {
    u8 *p = (u8 *)ptr;
    while (num--) {
        *p++ = (u8)value;
    }
    return ptr;
}