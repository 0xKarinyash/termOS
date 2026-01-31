// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OSCPU.h>
#include <syscall.h>
#include <types.h>
#include <IO/IOConsole.h>
#include <VM/PMM.h>
#include <VM/VMM.h>

#include <OS/Services/OSServiceProcess.h>
#include <OS/Services/OSServiceMemory.h>
#include <OS/Services/OSServiceIO.h>

static inline void WRMSR(UInt32 msr, UInt64 value) {
    UInt32 low = (UInt32)value;
    UInt32 high = (UInt32)(value >> 32);
    __asm__ volatile("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

static inline UInt64 RDMSR(UInt32 msr) {
    UInt32 low, high;
    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((UInt64)high << 32) | low;
}

extern void syscall_entry();

void OSServicesInitialize() {
    UInt64 efer = RDMSR(kHALModelSpecificRegisterExtendedFeatureEnable);
    WRMSR(kHALModelSpecificRegisterExtendedFeatureEnable, efer | 1); // Enabling SCE in EFER. Just enabling 0 bit

    // setting up STAR
    // 32:47 kernel selector (0x08)
    // 48:64 user selector (0x01 cuz sysret adds +16 for CS and +8 for SS)
    UInt64 star = (0x13ULL << 48) | (0x08ULL << 32); 
    WRMSR(kHALModelSpecificRegisterSystemCallTarget, star);

    WRMSR(kHALModelSpecificRegisterLongSystemCallTarget, (UInt64)syscall_entry); // setting handler adress

    // masking flags (IA32_FMASK)
    // 9 bit for finterrupts in syscall (IF) and few more necessary flags
    WRMSR(kHALModelSpecificRegisterSystemCallFlagMask, 0x200); // masking only IF

    if (gOSBootCPU.kernelStackPointer == 0) {
        void* physicalStackPointer = VMPhysicalMemoryAllocatePage();
        gOSBootCPU.kernelStackPointer = (UInt64)physicalStackPointer + HHDM_OFFSET + 4096;
    }

    WRMSR(kHALModelSpecificRegisterGSBase, (UInt64)&gOSBootCPU);
    WRMSR(kHALModelSpecificRegisterKernelGSBase, (UInt64)&gOSBootCPU);
}

UInt64 syscall_dispatch(UInt64 id, UInt64 arg1, UInt64 arg2, UInt64 arg3, UInt64 arg4, UInt64 arg5) {
    switch (id) {
        case SYS_EXIT:  return OSServiceProcessExit(arg1);
        case SYS_SPAWN: return OSServiceProcessSpawn((const char*)arg1);
        case SYS_MEM:   return OSServiceMemoryAllocate(arg1);
        case SYS_WRITE: return OSServiceIOWrite(arg1, arg2, arg3);
        case SYS_READ:  return OSServiceIORead(arg1, arg2, arg3);
        case SYS_WAIT:  return OSServiceProcessWait(arg1);
        default: 
            IOConsoleLog("[Dewar] Unknown syscall %d\n", id); 
            return -1;
    }
}