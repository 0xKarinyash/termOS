#pragma once

typedef unsigned _BitInt(32) bi_u32;
typedef unsigned _BitInt(64) bi_u64;

typedef struct {
    bi_u32 *base;
    bi_u64 base_size;
    bi_u64 width;
    bi_u64 height;
    bi_u64 pitch;
} Bootinfo;