// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#ifndef CONSOLE_H
#define CONSOLE_H

#include "shitgui.h"

void console_init(SG_Context *ctx);
void console_clear(u32 color);
SG_Context* console_get_context();
SG_Point console_get_dimensions();
void console_set_color(u32 color);
void console_set_cursor_pos(SG_Point *p);
void kprint(const char *str);
void kprintf(const char *fmt, ...);

#endif