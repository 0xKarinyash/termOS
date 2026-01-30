// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "core/scheduler.h"
#include <drivers/console.h>
#include <drivers/serial.h>
#include <drivers/font.h>
#include <drivers/shitgui.h>
#include <drivers/keyboard.h>

#include <core/math.h>
#include <types.h>

#include <stdarg.h>

#define COLOR_RED       0xFF5555
#define COLOR_VERYRED   0xFF0000
#define COLOR_GREEN     0x08bf39
#define COLOR_VERYGREEN 0x00FF00
#define COLOR_TURQUOISE 0x5effaf
#define COLOR_BLUE      0x5555FF
#define COLOR_VERYBLUE  0x0000FF
#define COLOR_LIGHTBLUE 0x3890e8
#define COLOR_YELLOW    0xFFFF55
#define COLOR_CYAN      0x55FFFF
#define COLOR_MAGENTA   0xFF55FF
#define COLOR_BLACK     0x000000
#define COLOR_WHITE     0xFFFFFF
#define COLOR_PINK      0xFFA3B1

static SG_Context *ctx_ptr = nullptr;
static SG_Point s_cursor_pos = {0};
static SG_Font s_font = {0};
static u32 s_color = COLOR_WHITE;
static u32 s_def_color = COLOR_WHITE;
static u32 s_bg_color = COLOR_BLACK;
static bool s_are_we_blinkin = true;
static bool s_waiting_for_input = false;
static bool s_cursor_visible = false;

void console_init(SG_Context *ctx) {
    ctx_ptr = ctx;
    s_cursor_pos.x = 0;
    s_cursor_pos.y = 0;

    s_font.h = 16;
    s_font.w = 8;
    s_font.base = (const unsigned char*)font8x16;
}

u64 console_get_colors() {
    return ((u64) s_color << 32) | s_bg_color;
}

void console_clear(u32 color) {
    if (!ctx_ptr) return;

    SG_Point p = {0, 0};
    sg_draw_rect(ctx_ptr, &p, ctx_ptr->width, ctx_ptr->height, color);
    
    s_cursor_pos.x = 0;
    s_cursor_pos.y = 0;
    s_bg_color = color;
}

SG_Context* console_get_context() {
    return ctx_ptr;
}

SG_Point console_get_dimensions() {
    SG_Point p = {0};
    p.x = ctx_ptr->width;
    p.y = ctx_ptr->height;
    return p;
}

void console_set_cursor_pos(SG_Point *p) {
    if (!p) return;
    s_cursor_pos.x = p->x;
    s_cursor_pos.y = p->y;
}

void console_putc(char c) {
    serial_writec(c);
    if (!ctx_ptr) return;
    if (c == '\n') {
        s_cursor_pos.x = 0;
        s_cursor_pos.y += s_font.h;
    } else if (c == '\t') {
        s_cursor_pos.x += s_font.w * 4;
    } else if (c == '\b') {
        s_cursor_pos.x -= s_font.w;
        sg_draw_rect(ctx_ptr, &s_cursor_pos, s_font.w, s_font.h, s_bg_color);
    } else {
        sg_draw_char_bitmap(ctx_ptr, &s_cursor_pos, c, s_color, &s_font);
        s_cursor_pos.x += s_font.w;
    }

    if (s_cursor_pos.x >= ctx_ptr->width) {
        s_cursor_pos.x = 0;
        s_cursor_pos.y += s_font.h;
    }
    if (s_cursor_pos.y >= ctx_ptr->height) {
        console_clear(s_bg_color); // TODO: do scrolling
    }
}

void kprint(const char *str) {
    for (i32 i = 0; str[i] != '\0'; i++) console_putc(str[i]);
}

static void print_dec(const i64 n) {
    if (n < 0) console_putc('-');
    u64 u = abs(n);
    char buffer[32];
    i32 i = 0;

    do {
        buffer[i] = (u % 10) + '0';
        u /= 10;
        i++;
    } while (u > 0);

    while (--i >= 0) {
        console_putc(buffer[i]);
    }
}

static void print_hex(u64 u, u8 padding) {
    console_putc('0');
    console_putc('x');

    if (u == 0) {
        console_putc('0');
        for (i32 i = 1; i < padding; i++) console_putc('0'); 
        return;
    }

    char buffer[16] = {0}; 
    i32 i = 0;

    do {
        i32 digit = u % 16;
        if (digit < 10) buffer[i++] = digit + '0';
        else            buffer[i++] = digit - 10 + 'A';
        u /= 16;
    } while (u > 0);

    while(i < padding) {
        console_putc('0');
        padding--;
    }

    while (--i >= 0) {
        console_putc(buffer[i]);
    }
}


void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (i32 i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    const char *str = va_arg(args, const char*);
                    if (!str) str = "undefined";
                    kprint(str);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    console_putc(c);
                    break;
                }
                case 'd': {
                    i32 num = va_arg(args, i32);
                    print_dec(num);
                    break;
                }
                case 'x': {
                    u64 num = va_arg(args, u64);
                    print_hex(num, 0);
                    break;    
                }
                case 'X' : {
                    u64 num = va_arg(args, u64);
                    print_hex(num, 16);
                    break;
                }
                default: {
                    console_putc(fmt[i]);
                    break;
                }
            }
        } else if (fmt[i] == '^') {
            i++;
            switch (fmt[i]) {
                case 'r': console_set_color(COLOR_RED); break;
                case 'R': console_set_color(COLOR_VERYRED); break;
                case 'g': console_set_color(COLOR_GREEN); break;
                case 'G': console_set_color(COLOR_VERYGREEN); break;
                case 't': console_set_color(COLOR_TURQUOISE); break;
                case 'b': console_set_color(COLOR_BLUE); break;
                case 'B': console_set_color(COLOR_VERYBLUE); break;
                case 'l': console_set_color(COLOR_LIGHTBLUE); break;
                case 'y': console_set_color(COLOR_YELLOW); break;
                case 'c': console_set_color(COLOR_CYAN); break;
                case 'm': console_set_color(COLOR_MAGENTA); break;
                case 'p': console_set_color(COLOR_PINK); break;
                case '0': console_set_color(COLOR_BLACK); break;
                case 'w': console_set_color(COLOR_WHITE); break;
                case '!': console_set_color(s_def_color); break;
                case '~': console_set_color(~s_bg_color); break;
                case '^': console_putc('^'); break;
                default: { 
                    console_putc('^');
                    console_putc(fmt[i]);
                    break; 
                }
            }
        } else {
            console_putc(fmt[i]);
        }
    }

    va_end(args);
}

void console_set_color(u32 color) {
    s_color = color;
}

void console_set_default_color(u32 color) {
    s_def_color = color;
}

char console_getc() {
    s_waiting_for_input = true;
    while (kb_buf.head == kb_buf.tail) { __asm__ volatile ("sti"); yield(1); }
    __asm__ volatile ("cli");
    s_waiting_for_input = false;
    char temp = kb_buf.buffer[kb_buf.tail];
    kb_buf.tail = (kb_buf.tail + 1) % KB_BUFF_SIZE;
    __asm__ volatile ("sti");
    if (s_cursor_visible) {
        cursor_blink();
        s_cursor_visible = false;
    }
    return temp;
}

void kgets(char* buff, u32 lim) {
    u32 i = 0;
    while (true) {
        char c = console_getc();
        switch (c) {
            case '\n': { 
                buff[i] = '\0';
                kprintf("\n");
                return;
            }
            case '\b': {
                if (i > 0) {
                    i--;
                    kprintf("\b \b");
                }
                break;
            }
            default: {
                if (i < lim - 1) { 
                    buff[i] = c;
                    kprintf("%c", c);
                    i++;
                }
            }
        }
    }
}

void cursor_blink() {
    sg_draw_rect(ctx_ptr, &s_cursor_pos, s_font.w, s_font.h, sg_get_pixel(ctx_ptr, &s_cursor_pos) ^ 0xFFFFFF);
}

void cursor_blinker_sched_task() {
    while (true) {
        if (s_are_we_blinkin) {
            if (s_waiting_for_input) {
                cursor_blink();
                s_cursor_visible = !s_cursor_visible;
            }
        } else if (s_cursor_visible) { 
            cursor_blink(); 
            s_cursor_visible = !s_cursor_visible;
        }
        yield(500);
    }
}

void console_toggle_cursor_blink() {
    s_are_we_blinkin = !s_are_we_blinkin;
}