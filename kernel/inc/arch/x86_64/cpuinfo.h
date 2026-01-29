// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <types.h>

#define CPU_FEAT_FPU      (1ULL << 0)
#define CPU_FEAT_TSC      (1ULL << 1)
#define CPU_FEAT_MSR      (1ULL << 2)
#define CPU_FEAT_APIC     (1ULL << 3)
#define CPU_FEAT_MTRR     (1ULL << 4)
#define CPU_FEAT_PAE      (1ULL << 5)

#define CPU_FEAT_SSE      (1ULL << 10)
#define CPU_FEAT_SSE2     (1ULL << 11)
#define CPU_FEAT_SSE3     (1ULL << 12)
#define CPU_FEAT_SSSE3    (1ULL << 13)
#define CPU_FEAT_SSE4_1   (1ULL << 14)
#define CPU_FEAT_SSE4_2   (1ULL << 15)

#define CPU_FEAT_AVX      (1ULL << 20)
#define CPU_FEAT_F16C     (1ULL << 21)
#define CPU_FEAT_RDRAND   (1ULL << 22)

#define CPU_FEAT_HYPERVISOR (1ULL << 30) 

typedef struct {
    i32 features;
    char vendor[13];
    u32 family;
    u32 model;
} cpu_info;

extern cpu_info g_cpu;

void cpuinfo_init();
bool cpu_has(u64 feature);