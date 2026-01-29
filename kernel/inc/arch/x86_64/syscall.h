// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#define MSR_EFER         0xC0000080
#define MSR_STAR         0xC0000081
#define MSR_LSTAR        0xC0000082
#define MSR_FMASK        0xC0000084
#define MSR_KERNEL_GS_BASE 0xC0000102

#define EFER_SCE         0x01 // System Call Enable

typedef enum {
    SYS_EXIT,
    SYS_EXEC,
    SYS_WRITE,
} syscalls;

void syscall_init();