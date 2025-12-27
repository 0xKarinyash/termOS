// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <drivers/shitgui.h>

#define SHITGUI_TRANSPARENCY_KEY 0xFF00FF

void sg_put_img(SG_Context *ctx, SG_Point *p, SG_Image *img) {
    if (!ctx->fb || !img) return;

    u32 start_x = p->x;
    u32 start_y = p->y;
    u32 draw_w = img->width;
    u32 draw_h = img->height;

    if (start_x >= ctx->width || start_y >= ctx->height) return;
    if (start_x + draw_w > ctx->width) draw_w = ctx->width - start_x;
    if (start_y + draw_h > ctx->height) draw_h = ctx->height - start_y;

    for (u32 y = 0; y < draw_h; y++) {
        volatile u32 *dest_ptr = &ctx->fb[(start_y + y) * ctx->pitch + start_x];
        u32 *src_ptr = &img->buffer[y * img->width];
        for (u32 x = 0; x < draw_w; x++) {
            u32 color = src_ptr[x];
            if (color != SHITGUI_TRANSPARENCY_KEY) dest_ptr[x] = color;
        }
    }
}

void sg_draw_rect(SG_Context *ctx, SG_Point *p, u32 w, u32 h, u32 color) {
    if (!ctx->fb) return;

    u32 start_x = p->x;
    u32 start_y = p->y;
    u32 draw_w = w;
    u32 draw_h = h;

    if (start_x >= ctx->width || start_y >= ctx->height) return;
    if (start_x + draw_w > ctx->width) draw_w = ctx->width - start_x;
    if (start_y + draw_h > ctx->height) draw_h = ctx->height - start_y;


    for (u32 y = 0; y < draw_h; y++) {
        volatile u32 *row_ptr = &ctx->fb[(start_y + y) * ctx->pitch + start_x];
        for (u32 x = 0; x < draw_w; x++) row_ptr[x] = color;
    }
}

void sg_draw_char_bitmap(SG_Context *ctx, SG_Point *p, char c, u32 color, SG_Font *font) {
    if (!ctx->fb) return;

    u32 start_x = p->x;
    u32 start_y = p->y;
    u32 draw_w = font->w;
    u32 draw_h = font->h;

    if (start_x >= ctx->width || start_y >= ctx->height) return;
    if (start_x + draw_w > ctx->width) draw_w = ctx->width - start_x;
    if (start_y + draw_h > ctx->height) draw_h = ctx->height - start_y;

    for (u32 y = 0; y < draw_h; y++) {
        u8 bitmap_row = font->base[(unsigned char)c * font->h + y];
        volatile u32 *row_ptr = &ctx->fb[(start_y + y) * ctx->pitch + start_x];
        for (u32 x = 0; x < draw_w; x++) {
            if (bitmap_row & (1 << (7 - x))) {
                row_ptr[x] = color;
            }
        }
    }
}