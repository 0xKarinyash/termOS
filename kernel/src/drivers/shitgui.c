// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <core/scheduler.h>
#include <core/panic.h>
#include <drivers/shitgui.h>
#include <mm/heap.h>
#include <mm/memory.h>

#define SHITGUI_TRANSPARENCY_KEY 0xFF00FF
#define SHITGUI_MAX_WINDOWS_AMOUNT 64

SG_Context* main_context;
SG_Window* windows[SHITGUI_MAX_WINDOWS_AMOUNT]; // нет блять линукс
u64 windows_amount = 0;

void sg_init(SG_Context *ctx) {
    main_context = ctx;
}

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

SG_Window* create_window(const char *title, SG_Point* size, SG_Point* position) {
    if (windows_amount >= SHITGUI_MAX_WINDOWS_AMOUNT) return nullptr; 
    SG_Context* ctx = nullptr;
    u32* fb = nullptr;
    SG_Window* window = nullptr;

    ctx = (SG_Context*)malloc(sizeof(SG_Context));
    if (!ctx) goto fail; // goto is GOOD for cleaning up in C stfu. 
                         // Mainline linux kernel contains 100k gotos

    fb = malloc(size->x * size->y * sizeof(u32));
    if (!fb) goto fail;
    memset(fb, 0, size->x * size->y * sizeof(u32));

    window = (SG_Window*)malloc(sizeof(SG_Window));
    if (!window) goto fail;

    ctx->fb = fb;
    ctx->height = size->y;
    ctx->width = size->x;
    ctx->pitch = size->x;
    
    
    window->ctx = ctx;
    window->title = title;
    window->pos.x = position->x;
    window->pos.y = position->y;

    windows[windows_amount] = window;
    windows_amount++;    

    return window;

fail:
    if (ctx) free(ctx);
    if (fb) free(fb);
    if (window) free(window);

    return nullptr;
}

void render_window(SG_Context *ctx, SG_Window *window) {
    SG_Image img = {0};
    img.buffer = window->ctx->fb;
    if (!img.buffer) panic("No img buffer found");
    img.height = window->ctx->height;
    img.width  = window->ctx->width;

    sg_put_img(ctx, &window->pos, &img);
}

void composer_task() {
    while (true) {
        for (u64 i = 0; i < windows_amount; i++) {
            render_window(main_context, windows[i]);
        }
        yield(1);
    }
}