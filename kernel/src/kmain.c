#include "../../common/bootinfo.h"
#include "console.h"
#include "types.h"

#include "shitgui.h"
#include "serial.h"
#include "panic.h" // IWYU pragma: keep
#include "../data/logo.h"

#include "gdt.h"
#include "idt.h"

int rectest(int a) {
    volatile int b = a + 1;
    return rectest(b * 2);
}

void kmain(Bootinfo* info) {
    u32 *fb = (u32*)info->framebuffer.base;
    if (!fb) return;

    SG_Context sg_ctx = {0};
    sg_ctx.fb = fb;
    sg_ctx.height = info->framebuffer.height;
    sg_ctx.width = info->framebuffer.width;
    sg_ctx.pitch = info->framebuffer.pitch;


    SG_Point start_pos = {75, 55}; // greeting
    serial_init();
    console_init(&sg_ctx);
    console_clear(0xFFFFFF);
    console_set_cursor_pos(&start_pos);
    console_set_color(0x000000);

    gdt_init();
    idt_init();

    SG_Point logo_point = {0, 10};
    sg_put_img(&sg_ctx, &logo_point, &logo_img);

    kprintf("Welcome to ^ptermOS^0!!!\n");
    SG_Point text_normal_point = {0, 120};  // not nice to hardcode nums like that but we have what we have
    console_set_cursor_pos(&text_normal_point);
    
    kprintf("MemoryMap located at ^g%x^0; \nMemory map size is ^g%x^0\n", (u64)info->mem.map, (u64)info->mem.map_size);

    // kfetch();

    // kprintf("I cant do anything yet lol");
  //  kprintf("stack overflow protection test");

   // rectest(0);

    // __asm__("ud2"); // panic :(

    while (1) { __asm__("hlt"); }
}