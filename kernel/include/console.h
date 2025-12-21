#ifndef CONSOLE_H
#define CONSOLE_H

#include "shitgui.h"

void console_init(SG_Context *ctx);
void console_set_color(u32 color);
void kprint(const char *str);

#endif