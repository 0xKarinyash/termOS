#include "../../common/bootinfo.h"
#include "types.h"

#include "shitgui.h"

void kmain(Bootinfo* info) {
    u32 *fb = (u32*)info->base;
    if (!fb) return;

    SG_Context sg_ctx = {0};
    sg_ctx.fb = fb;
    sg_ctx.height = info->height;
    sg_ctx.width = info->width;
    sg_ctx.pitch = info->pitch;

    SG_Point p = {0};
    p.x = 0;
    p.y = 0;

    u32 stripe_h = info->height / 5;

    sg_draw_rect(&sg_ctx, &p, info->width, stripe_h, 0x5BCEFA);
    p.y += stripe_h;
    sg_draw_rect(&sg_ctx, &p, info->width, stripe_h, 0xF5A9B8);
    p.y += stripe_h;
    sg_draw_rect(&sg_ctx, &p, info->width, stripe_h, 0xFFFFFF);
    p.y += stripe_h;
    sg_draw_rect(&sg_ctx, &p, info->width, stripe_h, 0xF5A9B8);
    p.y += stripe_h;
    sg_draw_rect(&sg_ctx, &p, info->width, stripe_h, 0x5BCEFA);

    while (1) { __asm__("hlt"); }
}