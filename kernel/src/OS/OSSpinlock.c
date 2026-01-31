// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/OSSpinlock.h>

void OSSpinlockLock(OSSpinlock* lock) {
    while (__atomic_exchange_n(&lock->lockValue, 1, __ATOMIC_ACQUIRE)) {
        while (lock->lockValue) {
            __builtin_ia32_pause();
        }
    }
}

void OSSpinlockUnlock(OSSpinlock *lock) {
    __atomic_store_n(&lock->lockValue, 0, __ATOMIC_RELEASE);
}

void OSSpinlockLockIRQ(OSSpinlock *lock, OSSpinlockState *state) {
    UInt64 rflags;
    __asm__ volatile("pushfq; pop %0; cli" : "=r"(rflags) :: "memory"); // read flags and cli
    state->interruptsEnabled = (rflags & (1 << 9)) != 0; // IF bit
    
    OSSpinlockLock(lock);
}

void OSSpinlockUnlockIRQ(OSSpinlock *lock, OSSpinlockState *state) {
    OSSpinlockUnlock(lock);
    if (state->interruptsEnabled) __asm__ volatile("sti" ::: "memory");
}