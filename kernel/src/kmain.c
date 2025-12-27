// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include "../../common/bootinfo.h"
#include "types.h"

#include "shitgui.h"
#include "serial.h"
#include "console.h"
#include "panic.h" // IWYU pragma: keep

#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"

#include "../data/logo.h"
#include "vmm.h"


int rectest(int a) {
    volatile int b = a + 1;
    kprintf("%d", b);
    return rectest(b * 2);
}

extern u64 _kernel_end;
extern u8* bitmap;
extern u64 bitmap_size_g;
extern u64* pml4_kernel;

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

    pmm_init(info->mem);
    
    kprintf("MemoryMap located at ^g%x^0 (^r%X^0); \
      \nMemory map size is ^g%x^0\
      \nKernel ends at ^g%x^0\
      \nBITMAP located at ^g%x^0 (^r%x^0)", (u64)info->mem.map, (u64)info->mem.map,(u64)info->mem.map_size, &_kernel_end, bitmap, bitmap_size_g);


    vmm_init(info);

    kprintf("\nIM ALIVE :D");

    // kprintf("\nSetting up guard page test");
    // u64* new_stack_phys = pmm_alloc_page();
    // u64 stack_top = 0x40000000;
    // vmm_map_page(pml4_kernel, (u64)new_stack_phys, stack_top, PTE_PRESENT | PTE_RW);
    
    // __asm__ volatile (
    //   "mov %0, %%rsp \n"      
    //   :: "r"(stack_top + 4096)
    // );

   // rectest(0);

    while (1) { __asm__("hlt"); }
}