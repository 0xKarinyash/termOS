// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <drivers/shitgui.h>

void console_init(SG_Context *ctx);
u64 console_get_colors();
void console_clear(u32 color);
SG_Context* console_get_context();
SG_Point console_get_dimensions();
void console_set_color(u32 color);
void console_set_default_color(u32 color);
void console_set_cursor_pos(SG_Point *p);
char console_getc();
void console_putc(char c);
void kprint(const char *str);
void kprintf(const char *fmt, ...);
void kgets(char* buff, u32 lim);
void cursor_blink();
void cursor_blinker_sched_task();
void console_toggle_cursor_blink();
