// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <drivers/console.h>
#include <core/rand.h>

#include "../data/cats.h"
#include "types.h"

const char* ascii_logo[] = {
    "      /\\___/\\     ", 
    "     |  > <  |     ", 
    "     |   w   |     ",
    "     |=======|__   ", 
    "     |       |  |  ", 
    "     |TERMOS |  |  ", 
    "     |       |__|  ",
    "     |_______|     " 
};


void cmd_kfetch() {
    kprintf("\n\n");
    kprintf("^p %s ^0\t\t^g kernel^0@^gtermos\n^0", ascii_logo[0]);
    kprintf("^p %s ^0\t\t^0-------------\n^0", ascii_logo[1]);   
    kprintf("^p %s ^0\t\t^gOS^0: termOS 0.0.1\n^0", ascii_logo[2]);   
    kprintf("^p %s ^0\t\t^gKernel^0: sucks\n^0", ascii_logo[3]);   
    kprintf("^p %s ^0\t\t^gUptime^0: 0h 0m\n^0", ascii_logo[4]);   
    kprintf("^p %s ^0\t\t^gShell^0: termosh\n^0", ascii_logo[5]);   
    kprintf("^p %s ^p\t\t^gDE^0: shitgui\n^0", ascii_logo[6]);   
    kprintf("^p %s ^p\t\t^gCPU^0: %s\n^0", ascii_logo[7], "cool one");
    kprintf("\n\n");
}

void cmd_meow() {
    u64 cats_count = sizeof(cats) / sizeof(cats[0]);
    u8 rand_num = shitrand() % cats_count;
    kprintf("Nyaaa!!\n\n%s\n\n", cats[rand_num]);
}

void cmd_help() {
    kprintf("\tWelcome to ^ptermOS^!'s kernel shell!\n");
    kprintf("\tIt can almost nothing! yet.\n");
}

void cmd_regs() {
    __asm__ volatile ("int3");
}

void print_regs(Registers *regs) {
    kprintf("--------------------------------\n");
    kprintf("\t\t^gREGISTERS^!\n");
    kprintf("--------------------------------\n");
    kprintf("^gRAX^!=%X, ^gRBX^!=%X\n", regs->rax, regs->rbx);
    kprintf("^gRCX^!=%X, ^gRDX^!=%X\n", regs->rcx, regs->rdx);
    kprintf("^gRSI^!=%X, ^gRDI^!=%X\n", regs->rsi, regs->rdi);
    kprintf("^gRBP^!=%X, ^gR8^! =%X\n", regs->rbp, regs->r8);
    kprintf("^gR9^! =%X, ^gR10^!=%X \n", regs->r9, regs->r10);
    kprintf("^gR11^!=%X, ^gR12^!=%X\n", regs->r11, regs->r12);
    kprintf("^gR13^!=%X, ^gR14^!=%X\n", regs->r13, regs->r14);
    kprintf("^gR15^!=%X\n",regs->r15);
    kprintf("--------------------------------\n");
}
