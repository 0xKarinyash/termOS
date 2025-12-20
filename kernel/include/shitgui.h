#ifndef SHITGUI_H
#define SHITGUI_H

#include "types.h"

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
    u32 font_w;
    u32 font_h;
} SG_FontMetrics;

void sg_put_img(SG_Context *ctx, SG_Point *p, SG_Image *img);
void sg_draw_rect(SG_Context *ctx, SG_Point *p, u32 w, u32 h, u32 color);

#endif