#pragma once
#ifndef GDT_H
#define GDT_H

#include "types.h"

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

void gdt_init();

#endif