// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

typedef struct {
    UInt16 offsetLow;
    UInt16 selector;
    UInt8  ist;   
    UInt8  attributes; 
    UInt16 offsetMid; 
    UInt32 offsetHigh; 
    UInt32 reserved;  
} __attribute__((packed)) HALInterruptsDescriptorTableEntry;

typedef struct {
    UInt16 limit;
    UInt64 base;
} __attribute__((packed)) HALInterruptsDescriptorTablePointer;

void HALInterruptsDescriptorTableInitialize();
