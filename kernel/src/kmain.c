#include "../../common/bootinfo.h"
#include "console.h"
#include "types.h"

#include "shitgui.h"
#include "serial.h"
#include "panic.h" // IWYU pragma: keep
#include "../data/logo.h"

#include "gdt.h"
#include "idt.h"

void kmain(Bootinfo* info) {
    u32 *fb = (u32*)info->framebuffer.base;
    if (!fb) return;

    SG_Context sg_ctx = {0};
    sg_ctx.fb = fb;
    sg_ctx.height = info->framebuffer.height;
    sg_ctx.width = info->framebuffer.width;
    sg_ctx.pitch = info->framebuffer.pitch;

    serial_init();
    console_init(&sg_ctx);
    console_clear(0xFFFFFF);
    console_set_color(0x000000);

    gdt_init();
    idt_init();

    SG_Point logo_point = {sg_ctx.width-100, 100};
    sg_put_img(&sg_ctx, &logo_point, &logo_img);

    kprint("Welcome to termOS!!!\n");
    kprintf("MemoryMap located at %x; \nMemory map size is %d\n", (u64)info->mem.map, (u64)info->mem.map_size);

    int a = 1;
    int b = 0;
    kprintf("%d", a/b);

    while (1) { __asm__("hlt"); }
}