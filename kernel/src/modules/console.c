#include "console.h"
#include "serial.h"
#include "font.h"
#include "shitgui.h"
#include "math.h"
#include "types.h"
#include <stdarg.h>


static SG_Context *ctx_ptr = nullptr;
static SG_Point s_cursor_pos = {0};
static SG_Font s_font = {0};
static u32 s_color = 0xFFFFFF;

void console_init(SG_Context *ctx) {
    ctx_ptr = ctx;
    s_cursor_pos.x = 0;
    s_cursor_pos.y = 0;

    s_font.h = 8;
    s_font.w = 8;
    s_font.base = (const unsigned char*)font8x8_basic;
}

void console_clear(u32 color) {
    if (!ctx_ptr) return;

    SG_Point p = {0, 0};
    sg_draw_rect(ctx_ptr, &p, ctx_ptr->width, ctx_ptr->height, color);
    
    s_cursor_pos.x = 0;
    s_cursor_pos.y = 0;
}

void console_set_cursor_pos(SG_Point *p) {
    if (!p) return;
    s_cursor_pos.x = p->x;
    s_cursor_pos.y = p->y;
}

static void console_putc(char c) {
    serial_writec(c);
    if (!ctx_ptr) return;
    if (c == '\n') {
        s_cursor_pos.x = 0;
        s_cursor_pos.y += s_font.h;
    } else {
        sg_draw_char_bitmap(ctx_ptr, &s_cursor_pos, c, s_color, &s_font);
        s_cursor_pos.x += s_font.w;
    }

    if (s_cursor_pos.x >= ctx_ptr->width) {
        s_cursor_pos.x = 0;
        s_cursor_pos.y += s_font.h;
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

static void print_hex(u64 u) {
    console_putc('0');
    console_putc('x');

    if (u == 0) {
        console_putc('0');
        return;
    }

    char buffer[16]; 
    i32 i = 0;

    while (u > 0) {
        i32 digit = u % 16;
        if (digit < 10) { buffer[i] = digit + '0'; }
        else { buffer[i] = digit - 10 + 'a'; }
        u /= 16;
        i++;
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
                    print_hex(num);
                    break;    
                }
                default: {
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