#ifndef TYPES_H
#define TYPES_H

typedef unsigned _BitInt(8)  u8;
typedef unsigned _BitInt(16) u16;
typedef unsigned _BitInt(32) u32;
typedef unsigned _BitInt(64) u64;

typedef signed _BitInt(8)    i8;
typedef signed _BitInt(16)   i16;
typedef signed _BitInt(32)   i32;
typedef signed _BitInt(64)   i64;

typedef u64 usize;
typedef u64 uintptr_t;

typedef struct {
    u64 rax, rbx, rcx, rdx, rbp, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15; // Pushed by us
    u64 int_no, err_code;                                                       // Pushed by macro
    u64 rip, cs, rflags, rsp, ss;                                               // Pushed by CPU
} Registers;

#endif