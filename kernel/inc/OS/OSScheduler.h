// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

typedef enum OSProcessState {
    kOSProcessStateDead,
    kOSProcessStateRunning,
    kOSProcessStateReady,
    kOSProcessStateBlocked,
    kOSProcessStateSleeping,
} OSProcessState;

typedef struct OSProcess {
    UInt64 processId;
    OSProcessState state;
    UInt64 physicalPML4;
    struct OSProcess* parent;
    char name[32];
    UInt64 heapStartPointer;
    UInt64 heapCurrentPointer;
} OSProcess;

typedef struct OSTask {
    UInt64 stackPointer;
    struct OSTask* next;
    UInt32 id;
    UInt32 sleepTicks;
    OSProcessState taskState; // reusing process_state cuz wn
    UInt64 kernelStackTop;
    OSProcess* process;
    Int32 waitingForProcess;
} OSTask;

void OSSchedulerInitialize();
OSTask* OSSchedulerSpawn(void(*entry)(), OSProcess* owner, Boolean isUser, UInt64 fixedUserStackPointer);
UInt64 OSSchedulerNext(UInt64 currentStackPointer);
void OSSchedulerYield(UInt64 ticks);
void OSSchedulerBlock(UInt32 processID);
void OSSchedulerWakeup(UInt32 processID);
void OSSchedulerTerminate(); // suicide

extern OSTask* gOSSchedulerCurrentTask;