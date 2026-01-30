// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

typedef unsigned char      UInt8;
typedef unsigned short     UInt16;
typedef unsigned int       UInt32;
typedef unsigned long long UInt64; 

typedef signed char        Int8;
typedef signed short       Int16;
typedef signed int         Int32;
typedef signed long long   Int64;

typedef UInt64 Size;
typedef UInt64 uintptr_t;

typedef struct {
    UInt64 rax, rbx, rcx, rdx, rbp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15; // Pushed by us
    UInt64 interruptNumber, errorCode;                                                       // Pushed by macro
    UInt64 rip, cs, rflags, rsp, ss;                                               // Pushed by CPU
} CPURegisters;


#define Boolean _Bool
