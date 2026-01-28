// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

typedef struct {
    u16 limit_low;  
    u16 base_low;     
    u8  base_middle;  
    u8  access; 
    u8  granularity;  
    u8  base_high;
} __attribute__((packed)) GDTDescriptor;


typedef struct {
    u16 limit; 
    u64 base; 
} __attribute__((packed)) GDTPtr;

typedef struct {
    u32 reserved0;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved1;
    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;
    u64 reserved2;
    u16 reserved3;
    u16 iomap_base;
} __attribute__((packed)) TSS;

extern TSS tss;

void gdt_init();
