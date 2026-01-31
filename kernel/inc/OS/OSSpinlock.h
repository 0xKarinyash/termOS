// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include <types.h>

typedef struct OSSpinlock {
    volatile UInt32 lockValue;
} OSSpinlock;

typedef struct OSSpinlockState {
    Boolean interruptsEnabled;
} OSSpinlockState;

void OSSpinlockLock(OSSpinlock* lock);
void OSSpinlockUnlock(OSSpinlock* lock);
void OSSpinlockLockIRQ(OSSpinlock* lock, OSSpinlockState* state);
void OSSpinlockUnlockIRQ(OSSpinlock *lock, OSSpinlockState* state);
