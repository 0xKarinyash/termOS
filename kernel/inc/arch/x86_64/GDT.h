// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

#define kHALGDTEntries 7

typedef struct {
    UInt16 limitLow;  
    UInt16 baseLow;     
    UInt8  baseMiddle;  
    UInt8  access; 
    UInt8  granularity;  
    UInt8  baseHigh;
} __attribute__((packed)) HALGlobalDescriptorTable;


typedef struct {
    UInt16 limit; 
    UInt64 base; 
} __attribute__((packed)) HALGlobalDescriptorTablePointer;

typedef struct {
    UInt32 reserved0;
    UInt64 rsp0;
    UInt64 rsp1;
    UInt64 rsp2;
    UInt64 reserved1;
    UInt64 ist1;
    UInt64 ist2;
    UInt64 ist3;
    UInt64 ist4;
    UInt64 ist5;
    UInt64 ist6;
    UInt64 ist7;
    UInt64 reserved2;
    UInt16 reserved3;
    UInt16 iomapBase;
} __attribute__((packed)) HALTaskStateSegment;


extern HALTaskStateSegment gHALTaskStateSegment;

void HALGlobalDescriptorTableInitialize();
