#ifndef IDT_H
#define IDT_H
#pragma once
#include "types.h"

typedef struct {
    u16 offset_low;
    u16 selector;
    u8  ist;   
    u8  attributes; 
    u16 offset_mid; 
    u32 offset_high; 
    u32 reserved;  
} __attribute__((packed)) IDTEntry;

typedef struct {
    u16 limit;
    u64 base;
} __attribute__((packed)) IDTPtr;

void idt_init();

#endif