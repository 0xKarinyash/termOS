#include "../../common/bootinfo.h"
#include "console.h"
#include "types.h"

#include "shitgui.h"
#include "serial.h"

void kmain(Bootinfo* info) {
    u32 *fb = (u32*)info->base;
    if (!fb) return;

    SG_Context sg_ctx = {0};
    sg_ctx.fb = fb;
    sg_ctx.height = info->height;
    sg_ctx.width = info->width;
    sg_ctx.pitch = info->pitch;

    serial_init();
    console_init(&sg_ctx);
    console_clear(0xFFFFFF);
    console_set_color(0x000000);
    kprint("Welcome to termOS!!!");

    while (1) { __asm__("hlt"); }
}