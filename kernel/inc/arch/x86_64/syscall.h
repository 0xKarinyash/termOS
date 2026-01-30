// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

enum {
    kHALModelSpecificRegisterExtendedFeatureEnable = 0xC0000080, // EFER
    kHALModelSpecificRegisterSystemCallTarget      = 0xC0000081, // STAR
    kHALModelSpecificRegisterLongSystemCallTarget  = 0xC0000082, // LSTAR
    kHALModelSpecificRegisterSystemCallFlagMask    = 0xC0000084, // FMASK
    kHALModelSpecificRegisterGSBase                = 0xC0000101, // GS_BASE
    kHALModelSpecificRegisterKernelGSBase          = 0xC0000102  // KERNEL_GS_BASE
};

enum {
    kHALEFERSystemCallExtensionsEnable = 0x01 // SCE
};

typedef enum {
    SYS_EXIT,
    SYS_SPAWN,
    SYS_MEM,
    SYS_WRITE,
    SYS_READ,
    SYS_WAIT,
} Services;

void OSServicesInitialize();