// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "bootinfo.h"
#include <shell/ksh.h>

#include <types.h>

#include <drivers/shitgui.h>
#include <drivers/serial.h>
#include <drivers/console.h>

#include <core/panic.h>
#include <core/splash.h>

#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

#define FG_COLOR 0xffffff
#define BG_COLOR 0x111111

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
    console_clear(BG_COLOR);
    console_set_color(FG_COLOR);
    console_set_default_color(FG_COLOR);

    gdt_init();
    idt_init();
    pic_remap();
    pmm_init(info->mem);
    vmm_init(info);

    show_splash(&sg_ctx);

    ksh(&sg_ctx);
    
    panic("Kernel main loop exited");
}