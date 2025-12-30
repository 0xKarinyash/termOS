// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// contents of this file will be changed CONSTANTLY
// im just testing new stuff here

#include "core/panic.h"
#include "drivers/shitgui.h"
#include <drivers/timer.h>
#include <drivers/console.h>
#include <types.h>
#include <shell/dbgcmd.h>

#include <core/scheduler.h>

SG_Window* win_a = nullptr;
SG_Window* win_b = nullptr;

void a() {
    SG_Point sizes = {0};
    sizes.x = win_a->ctx->width;
    sizes.y = win_a->ctx->height;

    u64 t = 0;
    while (true) {
        for (u32 y = 0; y < sizes.y; y++) {
            for (u32 x = 0; x < sizes.x; x++) {
                win_a->ctx->fb[y * sizes.x + x] = (x * y) ^ t;
            }
        }
        t++;
    }
}
void b() {
    SG_Point sizes = {0};
    sizes.x = win_b->ctx->width;
    sizes.y = win_b->ctx->height;

    u64 t = 0;
    while (true) {
        for (u32 y = 0; y < sizes.y; y++) {
            for (u32 x = 0; x < sizes.x; x++) {
                win_b->ctx->fb[y * sizes.x + x] = (x ^ y) ^ t;
            }
        }
        t++;
    }
}

u64 debug() {
    kprintf("scheduler test\n");
    SG_Point sizes = {0};
    sizes.x = 256;
    sizes.y = 256;
    SG_Point pos_a = {0};
    pos_a.x = 100;
    pos_a.y = 40;
    SG_Point pos_b = {0};
    pos_b.x = 512;
    pos_b.y = 40;
    win_a = create_window("A", &sizes, &pos_a);
    if (!win_a) panic("No win a");
    win_b = create_window("B", &sizes, &pos_b);
    if (!win_b) panic("No win b");
    sched_spawn(a);
    sched_spawn(b);
    return 0;
}