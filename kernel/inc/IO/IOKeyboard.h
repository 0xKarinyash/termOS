// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>
#include <OS/OSSpinlock.h>

enum {
    kIOKeyboardBufferSize = 256,
};

typedef struct { 
    OSSpinlock lock;
    char buffer[kIOKeyboardBufferSize];
    volatile UInt16 head;
    volatile UInt16 tail;
} IOKeyboardController;

extern IOKeyboardController gIOKeyboardController;

void IOKeyboardInterruptsHandler();
char IOKeyboardGetCharacter();