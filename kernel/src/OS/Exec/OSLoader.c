// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/Exec/HOTLoader.h>
#include <OS/Exec/OSLoader.h>
#include <OS/OSPanic.h>
#include <OS/OSScheduler.h>
#include <lib/String.h>


#include <VM/PMM.h>
#include <VM/VMM.h>
#include <VM/Heap.h>

#include <FS/VFS.h>

#include <IO/IOConsole.h>
#include <types.h>

extern UInt32 gOSSchedulerNextProcessID;

enum {
    kOSUserStackTop = 0x70000000,
    kOSHeapStart = 0x40000000
};


Int32 OSLoaderProcessSpawn(const char* executablePath, const char* processName) {
    FSVNode* executableFile = FSVirtualFileSystemOpenPath(executablePath);
    if (!executableFile) {
        return -1;
    }

    OSProcess* newProcess = (OSProcess*)VMHeapAllocate(sizeof(OSProcess));
    if (!newProcess) {
        return -2;
    }

    MemorySet(newProcess, 0, sizeof(OSProcess));
    newProcess->processId = gOSSchedulerNextProcessID++; 
    newProcess->state = kOSProcessStateRunning; 
    newProcess->physicalPML4 = VMVirtualMemoryCreateAddressSpace();
    newProcess->heapStartPointer = kOSHeapStart;
    newProcess->heapCurrentPointer = kOSHeapStart;
    StringCopyWithLimit(newProcess->name, processName, 31);

    UInt8* imageBuffer = (UInt8*)VMHeapAllocate(executableFile->dataLength);
    if (!imageBuffer) {
        VMHeapFree(newProcess);
        return -3;
    }

    FSVNodeRead(executableFile, 0, executableFile->dataLength, imageBuffer);

    UInt64 entryPoint = HOTLoad(newProcess, imageBuffer);
    if (!entryPoint) {
        VMHeapFree(imageBuffer);
        VMHeapFree(newProcess);
        return -4;
    }

    VMHeapFree(imageBuffer);

    VMVirtualMemorySetupUserStack((UInt64*)newProcess->physicalPML4);
    
    if (!OSSchedulerSpawn((void(*)())entryPoint, newProcess, true, kOSUserStackTop)) {
        VMHeapFree(newProcess);
        return -5;
    }

    return newProcess->processId;
}

void init_task_entry() {
    Int32 pid = OSLoaderProcessSpawn("/System/CoreServices/init", "init");
    if (pid < 0) {
        OSPanic("FATAL: Failed to spawn /System/CoreServices/init");
    }

    while (1) { __asm__("sti; hlt"); }
}