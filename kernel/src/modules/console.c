#include "console.h"
#include "serial.h"
#include "font.h"
#include "shitgui.h"

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
    for (int i = 0; str[i] != '\0'; i++) console_putc(str[i]);
}

void console_set_color(u32 color) {
    s_color = color;
}