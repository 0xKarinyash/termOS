// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "bootinfo.h"
#include "../data/logo.h"

#include <types.h>

#include <drivers/shitgui.h>
#include <drivers/serial.h>
#include <drivers/console.h>

#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

extern u64 _kernel_end;

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

    gdt_init();
    idt_init();
    pic_remap();
    pmm_init(info->mem);
    vmm_init(info);

    SG_Point start_pos = {75, 55}; // greeting
    console_clear(0xFFFFFF);
    console_set_cursor_pos(&start_pos);
    console_set_color(0x000000);

    SG_Point logo_point = {0, 10};
    sg_put_img(&sg_ctx, &logo_point, &logo_img);

    kprintf("Welcome to ^ptermOS^0!!!\n");
    SG_Point text_normal_point = {0, 120};  // not nice to hardcode nums like that but we have what we have
    console_set_cursor_pos(&text_normal_point);

    kprintf("ksh_> ");
    char buff[32];
    kgets(buff, 32);
    kprintf("You typed: %s", &buff);

    __asm__ volatile ("sti");

    while (1) { __asm__("hlt"); }
}