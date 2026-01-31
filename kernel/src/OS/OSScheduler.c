// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/OSScheduler.h>
#include <OS/OSSpinlock.h>
#include <OS/OSPanic.h>
#include <lib/String.h>
#include <VM/Heap.h>
#include <VM/VMM.h>
#include <OSCPU.h>
#include <GDT.h>

OSTask* gOSSchedulerCurrentTask = nullptr;
UInt32 gOSSchedulerNextProcessID = 1;

extern void irq0_handler();
extern UInt64 gVMKernelPML4Physical;

static OSProcess sOSSchedulerKernelProcess;
static OSSpinlock sOSSchedulerLock;

static OSTask* sOSSchedulerGarbageCollectorTask = nullptr;

void idle_task() {
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void OSSchedulerGarbageCollector() {
    OSTask* self = gOSSchedulerCurrentTask;

    while (true) {
        OSTask* previousTask = self;
        OSTask* nextTask = previousTask->next;

        while (nextTask != self) {
            if (nextTask->taskState == kOSProcessStateDead) {
                OSSpinlockState lockState;
                OSSpinlockLockIRQ(&sOSSchedulerLock, &lockState);
                
                previousTask->next = nextTask->next;
                
                OSSpinlockUnlockIRQ(&sOSSchedulerLock, &lockState);

                if (nextTask->kernelStackBase) {
                    VMHeapFree(nextTask->kernelStackBase);
                }
                VMHeapFree(nextTask);

                nextTask = previousTask->next;
            } else {
                previousTask = nextTask;
                nextTask = nextTask->next;
            }
        }

        OSSchedulerYield(200);
    }
}

static void sOSSchedulerPostGarbageCollectorSignal() {
    if (!sOSSchedulerGarbageCollectorTask) return;

    OSSpinlockState state;
    OSSpinlockLockIRQ(&sOSSchedulerLock, &state);

    sOSSchedulerGarbageCollectorTask->taskState = kOSProcessStateRunning;
    sOSSchedulerGarbageCollectorTask->sleepTicks = 0;

    OSSpinlockUnlockIRQ(&sOSSchedulerLock, &state);
}

void OSSchedulerInitialize() {
    sOSSchedulerKernelProcess.processId = 0;
    sOSSchedulerKernelProcess.state = kOSProcessStateRunning;
    sOSSchedulerKernelProcess.physicalPML4 = gVMKernelPML4Physical;
    StringCopy(sOSSchedulerKernelProcess.name, "kernel");

    OSTask* kernelTask = (OSTask*)VMHeapAllocate(sizeof(OSTask));
    if (!kernelTask) OSPanic("Failed to initialize scheduler: OOm");

    MemorySet(kernelTask, 0, sizeof(OSTask));

    kernelTask->id = 0;
    kernelTask->process = &sOSSchedulerKernelProcess;
    kernelTask->sleepTicks = 0;
    kernelTask->next = kernelTask;
    kernelTask->taskState = kOSProcessStateRunning;
    kernelTask->waitingForProcess = -1;

    gOSSchedulerCurrentTask = kernelTask;
    OSSchedulerSpawn(idle_task, &sOSSchedulerKernelProcess, false, 0);
    sOSSchedulerGarbageCollectorTask = OSSchedulerSpawn(OSSchedulerGarbageCollector, &sOSSchedulerKernelProcess, false, 0);
    return;
}

OSTask* OSSchedulerSpawn(void(*entry)(), OSProcess* owner, Boolean isUser, UInt64 fixedUserStackPointer) {
    OSTask* task = (OSTask*)VMHeapAllocate(sizeof(OSTask));
    if (!task) return nullptr;
    if (!owner) owner = &sOSSchedulerKernelProcess;

    UInt64 stackSize = 16384;
    UInt8* stackBaseAddress = (UInt8*)VMHeapAllocate(stackSize);
    if (!stackBaseAddress) {
        sOSSchedulerPostGarbageCollectorSignal();
        OSSchedulerYield(0);
        stackBaseAddress = (UInt8*)VMHeapAllocate(stackSize);
        if (!stackBaseAddress) {
            VMHeapFree(task);
            return nullptr;
        }
    }
    UInt64* rsp = (UInt64*)(stackBaseAddress + stackSize); 

    UInt64 cs = isUser ? 0x23 : 0x08;
    UInt64 ss = isUser ? 0x1b : 0x10;
    UInt64 rflags = 0x202;
    UInt64 targetStackPointer = 0;
    if (isUser) targetStackPointer = fixedUserStackPointer;
    else targetStackPointer = (UInt64)stackBaseAddress + stackSize;

    *--rsp = ss; // SS -- Kernel data
    *--rsp = targetStackPointer; // rsp
    *--rsp = rflags; // RFLAGS -- Interrupts Enabled | Reserved bit;
    *--rsp = cs; // CS -- Kernel Code;
    *--rsp = (UInt64)entry; // RIP

    *--rsp = 0; // int no
    *--rsp = 0; // err code

    for (UInt8 i = 0; i < 15; i++) *--rsp = 0; // R15 .. RAX

    task->stackPointer = (UInt64)rsp;
    task->process = owner;
    task->id = owner->processId;
    task->sleepTicks = 0;
    task->kernelStackTop = (UInt64)stackBaseAddress + stackSize;
    task->kernelStackBase = stackBaseAddress;
    task->taskState = kOSProcessStateRunning;
    task->waitingForProcess = -1;

    OSSpinlockState state;
    OSSpinlockLockIRQ(&sOSSchedulerLock, &state);

    task->next = gOSSchedulerCurrentTask->next;
    gOSSchedulerCurrentTask->next = task;

    OSSpinlockUnlockIRQ(&sOSSchedulerLock, &state);

    return task;
}

UInt64 OSSchedulerNext(UInt64 currentStackPointer) {
    if (!gOSSchedulerCurrentTask) return currentStackPointer;

    OSSpinlockLock(&sOSSchedulerLock);

    gOSSchedulerCurrentTask->stackPointer = currentStackPointer;
    OSTask* taskIterator = gOSSchedulerCurrentTask->next;

    do {
        if (taskIterator->sleepTicks > 0) taskIterator->sleepTicks--;
        taskIterator = taskIterator->next;
    } while (taskIterator != gOSSchedulerCurrentTask->next);

    if (gOSSchedulerCurrentTask->sleepTicks > 0) gOSSchedulerCurrentTask->sleepTicks--;

    OSTask* nextTask = gOSSchedulerCurrentTask->next;
    while (1) {
        if (nextTask->taskState == kOSProcessStateSleeping && nextTask->sleepTicks == 0) nextTask->taskState = kOSProcessStateRunning;
        if (nextTask->taskState == kOSProcessStateRunning) break;
        nextTask = nextTask->next;

        if (nextTask == gOSSchedulerCurrentTask) {
            if (gOSSchedulerCurrentTask->taskState == kOSProcessStateRunning) break;
            OSPanic("no running tasks");
        }
    }

    if (nextTask->process->physicalPML4 != gOSSchedulerCurrentTask->process->physicalPML4) VMLoadCR3(nextTask->process->physicalPML4);
    gOSSchedulerCurrentTask = nextTask;
    gHALTaskStateSegment.rsp0 = gOSSchedulerCurrentTask->kernelStackTop;
    gOSBootCPU.kernelStackPointer = gOSSchedulerCurrentTask->kernelStackTop;

    OSSpinlockUnlock(&sOSSchedulerLock);
    return gOSSchedulerCurrentTask->stackPointer;
}

void OSSchedulerBlock(UInt32 processID) {
    gOSSchedulerCurrentTask->taskState = kOSProcessStateBlocked;
    gOSSchedulerCurrentTask->waitingForProcess = processID;
    __asm__ volatile("int $32");
}

void OSSchedulerWakeup(UInt32 processID) {
    OSSpinlockState state;
    OSSpinlockLockIRQ(&sOSSchedulerLock, &state);

    OSTask* iteratorTask = gOSSchedulerCurrentTask;
    do {
        if (iteratorTask->taskState == kOSProcessStateBlocked && iteratorTask->waitingForProcess == (Int32)processID) {
            iteratorTask->taskState = kOSProcessStateRunning;
            iteratorTask->waitingForProcess = -1;
        }
        iteratorTask = iteratorTask->next;
    } while (iteratorTask != gOSSchedulerCurrentTask);

    OSSpinlockUnlockIRQ(&sOSSchedulerLock, &state);
}


void OSSchedulerTerminate() {
    UInt32 processID = gOSSchedulerCurrentTask->id;
    gOSSchedulerCurrentTask->taskState = kOSProcessStateDead;

    OSSchedulerWakeup(processID);
    sOSSchedulerPostGarbageCollectorSignal();
    __asm__ volatile("int $32");
}

void OSSchedulerYield(UInt64 ticks) {
    gOSSchedulerCurrentTask->sleepTicks = ticks;
    gOSSchedulerCurrentTask->taskState = kOSProcessStateSleeping;
    __asm__ volatile("int $32");
}
