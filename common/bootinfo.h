// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once

typedef unsigned int       BIUInt32;
typedef unsigned long long BIUInt64; 

typedef struct {
    BIUInt32* base;
    BIUInt64 baseSize;
    BIUInt64 width;
    BIUInt64 height;
    BIUInt64 pitch;
} BIFramebuffer;

typedef struct {
    void* map;             
    BIUInt64 mapSize;       
    BIUInt64 descriptorSize;
    BIUInt32 mapKey;
    BIUInt32 descriptorVersion;
} BIMemoryMap;

typedef struct {
    void* address;
    BIUInt64 size;
} BIInitramfs;

typedef struct {
    BIUInt64 magic;
    BIFramebuffer framebuffer;
    BIMemoryMap memoryMap;
    BIInitramfs initramfs;
} Bootinfo;

#define BOOTINFO_MAGIC 0x7465726D // term