// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <syscalls/io.h>
#include <drivers/console.h>

u64 sys_write(u64 fd, u64 buff, u64 len) {
    if (fd == 1 || fd == 2) {
        char* str = (char*)buff;
        for (u64 i = 0; i < len; i++) {
            console_putc(str[i]);
        }
        return len;
    }
    return 0;
}

u64 sys_read(u64 fd, u64 buff, u64 count) {
    char* buf = (char*)buff;
    if (fd == 0) {
        for (u64 i = 0; i < count; i++) {
            buf[i] = console_getc();
        }
        return count;
    }
    return 0;
}