// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#pragma once
#include <types.h>

typedef enum {
    kCPUFeatureFPU        = (1ULL << 0),
    kCPUFeatureTSC        = (1ULL << 1),
    kCPUFeatureMSR        = (1ULL << 2),
    kCPUFeatureAPIC       = (1ULL << 3),
    kCPUFeatureMTRR       = (1ULL << 4),
    kCPUFeaturePAE        = (1ULL << 5),
    
    kCPUFeatureSSE        = (1ULL << 10),
    kCPUFeatureSSE2       = (1ULL << 11),
    kCPUFeatureSSE3       = (1ULL << 12),
    kCPUFeatureSSSE3      = (1ULL << 13),
    kCPUFeatureSSE4_1     = (1ULL << 14),
    kCPUFeatureSSE4_2     = (1ULL << 15),
    
    kCPUFeatureAVX        = (1ULL << 20),
    kCPUFeatureF16C       = (1ULL << 21),
    kCPUFeatureRDRAND     = (1ULL << 22),
    
    kCPUFeatureHypervisor = (1ULL << 30)
} OSCPUFeature;

typedef struct {
    UInt64 userStackPointer;
    UInt64 kernelStackPointer;
    UInt64 selfAddress;

    UInt64 features;
    char vendorID[13];
    UInt32 family;
    UInt32 model;
} OSCPUData;

extern OSCPUData gOSBootCPU;

void OSCPUInitialize(UInt64 kernelStackTop);
Boolean OSCPUHasFeature(OSCPUFeature feature);