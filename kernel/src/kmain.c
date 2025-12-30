// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "bootinfo.h"
#include "core/scheduler.h"
#include <shell/ksh.h>

#include <types.h>

#include <drivers/shitgui.h>
#include <drivers/serial.h>
#include <drivers/console.h>
#include <drivers/timer.h>

#include <core/panic.h>
#include <core/splash.h>

#include <gdt.h>
#include <idt.h>
#include <pic.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>

#define FG_COLOR 0xffffff
#define BG_COLOR 0x111111

extern u64 _kernel_end;
static SG_Context sg_ctx;

void kmain(Bootinfo* info) {
    serial_init();
    serial_write("Kernel started\n");

    console_init(&sg_ctx);

    gdt_init();
    kprintf("GDT initialized\n");
    idt_init();
    kprintf("IDT initialized\n");
    pic_remap();
    kprintf("PIC remapped\n");
    pmm_init(&info->mem);
    kprintf("PMM initialized\n");
    vmm_init(info);
    kprintf("VMM initialized\n");
    timer_init(1000);
    kprintf("Timer initialized\n");
    heap_init();
    kprintf("Heap initialized\n");
    sched_init();
    kprintf("Scheduler initialized\n");
    sg_init(&sg_ctx);
    kprintf("Shitgui initialized");

    info = (Bootinfo*)PHYS_TO_HHDM((u64)info);

    u32 *fb = (u32*)info->framebuffer.base;
    if (!fb) return serial_write("No framebuffer found!!");

    sg_ctx.fb = fb;
    sg_ctx.height = info->framebuffer.height;
    sg_ctx.width = info->framebuffer.width;
    sg_ctx.pitch = info->framebuffer.pitch;

    console_clear(BG_COLOR);
    console_set_color(FG_COLOR);
    console_set_default_color(FG_COLOR);

    show_splash(&sg_ctx);

    sched_spawn(composer_task);
    sched_spawn(ksh);
    __asm__ volatile("sti");
    
    while (true) __asm__ volatile("hlt");
    panic("How in the name of God you got here?");
}