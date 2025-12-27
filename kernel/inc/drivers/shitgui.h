// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#ifndef SHITGUI_H
#define SHITGUI_H

#include <types.h>

typedef struct {
    volatile u32 *fb;
    u32 width;
    u32 height;
    u32 pitch;
} SG_Context;
typedef struct { 
    u32 x;
    u32 y;
} SG_Point;
typedef struct {
    u32 *buffer;
    u32 height;
    u32 width;
} SG_Image;

typedef struct {
    u32 w;
    u32 h;
    const unsigned char* base;
} SG_Font;

void sg_put_img(SG_Context *ctx, SG_Point *p, SG_Image *img);
void sg_draw_rect(SG_Context *ctx, SG_Point *p, u32 w, u32 h, u32 color);
void sg_draw_char_bitmap(SG_Context *ctx, SG_Point *p, char c, u32 color, SG_Font *font);

#endif