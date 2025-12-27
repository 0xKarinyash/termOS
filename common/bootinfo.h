// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once

typedef unsigned _BitInt(32) bi_u32;
typedef unsigned _BitInt(64) bi_u64;

typedef struct {
    bi_u32 *base;
    bi_u64 base_size;
    bi_u64 width;
    bi_u64 height;
    bi_u64 pitch;
} BI_Framebuffer;

typedef struct {
    void *map;             
    bi_u64 map_size;       
    bi_u64 descriptor_size;
    bi_u32 map_key;
    bi_u32 descriptor_version;
} BI_MemoryMap;

typedef struct {
    BI_Framebuffer framebuffer;
    BI_MemoryMap mem;
} Bootinfo;