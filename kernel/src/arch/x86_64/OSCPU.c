// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <types.h>
#include <OSCPU.h>

#define MSR_GS_BASE         0xC0000101
#define MSR_KERNEL_GS_BASE  0xC0000102

OSCPUData gOSBootCPU = {0};

static inline void CPUID(UInt32 leaf, UInt32 subleaf, UInt32 *eax, UInt32 *ebx, UInt32 *ecx, UInt32 *edx) {
    __asm__ volatile("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(subleaf)
    );
}

static inline void WRMSR(UInt32 msr, UInt64 val) {
    UInt32 low = (UInt32)val;
    UInt32 high = (UInt32)(val >> 32);
    __asm__ volatile("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

void OSCPUInitialize(UInt64 kernelStackTop) {
    gOSBootCPU.kernelStackPointer = kernelStackTop;
    gOSBootCPU.selfAddress = (UInt64)&gOSBootCPU;
    WRMSR(MSR_KERNEL_GS_BASE, (UInt64)&gOSBootCPU);


    UInt32 eax, ebx, ecx, edx;

    CPUID(0, 0, &eax, &ebx, &ecx, &edx);
    
    UInt32* vendorPointer = (UInt32*)gOSBootCPU.vendorID;
    vendorPointer[0] = ebx;
    vendorPointer[1] = edx;
    vendorPointer[2] = ecx;
    gOSBootCPU.vendorID[12] = '\0';

    CPUID(1, 0, &eax, &ebx, &ecx, &edx);

    gOSBootCPU.family = (eax >> 8) & 0xF;
    gOSBootCPU.model  = (eax >> 4) & 0xF;
    if (gOSBootCPU.family == 6 || gOSBootCPU.family == 15) {
        gOSBootCPU.model += ((eax >> 16) & 0xF) << 4;
    }

    if (edx & (1 << 0))  gOSBootCPU.features |= kCPUFeatureFPU;
    if (edx & (1 << 4))  gOSBootCPU.features |= kCPUFeatureTSC;
    if (edx & (1 << 5))  gOSBootCPU.features |= kCPUFeatureMSR;
    if (edx & (1 << 6))  gOSBootCPU.features |= kCPUFeaturePAE;
    if (edx & (1 << 9))  gOSBootCPU.features |= kCPUFeatureAPIC;
    if (edx & (1 << 12)) gOSBootCPU.features |= kCPUFeatureMTRR;

    if (edx & (1 << 25)) gOSBootCPU.features |= kCPUFeatureSSE;
    if (edx & (1 << 26)) gOSBootCPU.features |= kCPUFeatureSSE2;
    if (ecx & (1 << 0))  gOSBootCPU.features |= kCPUFeatureSSE3;
    if (ecx & (1 << 9))  gOSBootCPU.features |= kCPUFeatureSSSE3;
    if (ecx & (1 << 19)) gOSBootCPU.features |= kCPUFeatureSSE4_1;
    if (ecx & (1 << 20)) gOSBootCPU.features |= kCPUFeatureSSE4_2;

    if (ecx & (1 << 28)) gOSBootCPU.features |= kCPUFeatureAVX;
    if (ecx & (1 << 29)) gOSBootCPU.features |= kCPUFeatureF16C;
    if (ecx & (1 << 30)) gOSBootCPU.features |= kCPUFeatureRDRAND;
    
    if (ecx & (1 << 31)) gOSBootCPU.features |= kCPUFeatureHypervisor;
}

Boolean OSCPUHasFeature(OSCPUFeature feature) {
    return (gOSBootCPU.features & feature) != 0;
}