// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

enum {
    kIOKeyboardBufferSize = 256,
};

typedef struct { 
    char buffer[kIOKeyboardBufferSize];
    UInt16 head;
    UInt16 tail;
} IOKeyboardBuffer;

extern IOKeyboardBuffer gIOKeyboardInputBuffer;

void IOKeyboardInterruptsHandler();
