// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once

typedef unsigned int       bi_u32;
typedef unsigned long long bi_u64; 

typedef struct {
    bi_u32* base;
    bi_u64 base_size;
    bi_u64 width;
    bi_u64 height;
    bi_u64 pitch;
} BI_Framebuffer;

typedef struct {
    void* map;             
    bi_u64 map_size;       
    bi_u64 descriptor_size;
    bi_u32 map_key;
    bi_u32 descriptor_version;
} BI_MemoryMap;

typedef struct {
    void* addr;
    bi_u64 size;
} BI_Initramfs;

typedef struct {
    bi_u64 magic;
    BI_Framebuffer framebuffer;
    BI_MemoryMap mem;
    BI_Initramfs initramfs;
} Bootinfo;

#define BOOTINFO_MAGIC 0x7465726D // term