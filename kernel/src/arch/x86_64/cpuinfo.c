// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include "types.h"
#include <cpuinfo.h>

cpu_info g_cpu = {0};

static inline void cpuid(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx) {
    __asm__ volatile("cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(subleaf)
    );
}

void cpuinfo_init() {
    u32 eax, ebx, ecx, edx;

    cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    
    u32* vendor_ptr = (u32*)g_cpu.vendor;
    vendor_ptr[0] = ebx;
    vendor_ptr[1] = edx;
    vendor_ptr[2] = ecx;
    g_cpu.vendor[12] = '\0';

    cpuid(1, 0, &eax, &ebx, &ecx, &edx);

    g_cpu.family = (eax >> 8) & 0xF;
    g_cpu.model  = (eax >> 4) & 0xF;
    if (g_cpu.family == 6 || g_cpu.family == 15) {
        g_cpu.model += ((eax >> 16) & 0xF) << 4;
    }

    if (edx & (1 << 0))  g_cpu.features |= CPU_FEAT_FPU;
    if (edx & (1 << 4))  g_cpu.features |= CPU_FEAT_TSC;
    if (edx & (1 << 5))  g_cpu.features |= CPU_FEAT_MSR;
    if (edx & (1 << 6))  g_cpu.features |= CPU_FEAT_PAE;
    if (edx & (1 << 9))  g_cpu.features |= CPU_FEAT_APIC;
    if (edx & (1 << 12)) g_cpu.features |= CPU_FEAT_MTRR;
    if (edx & (1 << 25)) g_cpu.features |= CPU_FEAT_SSE;
    if (edx & (1 << 26)) g_cpu.features |= CPU_FEAT_SSE2;

    if (ecx & (1 << 0))  g_cpu.features |= CPU_FEAT_SSE3;
    if (ecx & (1 << 9))  g_cpu.features |= CPU_FEAT_SSSE3;
    if (ecx & (1 << 19)) g_cpu.features |= CPU_FEAT_SSE4_1;
    if (ecx & (1 << 20)) g_cpu.features |= CPU_FEAT_SSE4_2;
    if (ecx & (1 << 28)) g_cpu.features |= CPU_FEAT_AVX;
    if (ecx & (1 << 29)) g_cpu.features |= CPU_FEAT_F16C;
    
    if (ecx & (1 << 30)) g_cpu.features |= CPU_FEAT_RDRAND;
    
    if (ecx & (1 << 31)) g_cpu.features |= CPU_FEAT_HYPERVISOR;
}

bool cpu_has(u64 feature) {
    return (g_cpu.features & feature) != 0;
}