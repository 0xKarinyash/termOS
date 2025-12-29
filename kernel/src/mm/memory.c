// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <mm/memory.h>
#include <types.h>

void *memset(void *ptr, int value, usize num) {
    u8 *p = (u8 *)ptr;
    while (num--) {
        *p++ = (u8)value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, u64 n) {
    u8* d = (u8*)dest;
    const u8* s = (const u8*)src;

    while (n >= 8) {
        *(u64*)d = *(const u64*)s;
        d += 8;
        s += 8;
        n -= 8;
    }

    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dest;
}