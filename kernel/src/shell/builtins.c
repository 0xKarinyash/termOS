// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash


#include <types.h>
#include <cpuinfo.h>

#include <drivers/console.h>
#include <drivers/timer.h>

#include <core/rand.h>
#include <core/loader.h>
#include <core/scheduler.h>
#include <core/string.h>

#include <mm/vmm.h>
#include <mm/heap.h>

#include <shell/dbgcmd.h>

#include "../data/cats.h"

extern task* curr_task;

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
    u64 uptime_s = get_uptime() / 1000;

    kprintf("\n\n");
    kprintf("^p %s ^!\t\t^g kernel^!@^gtermos\n^0", ascii_logo[0]);
    kprintf("^p %s ^!\t\t^!-------------\n^!", ascii_logo[1]);
    kprintf("^p %s ^!\t\t^gOS^!: termOS %s\n^!", ascii_logo[2], TERMOS_VERSION);   
    kprintf("^p %s ^!\t\t^gKernel^!: Dewar (x86_64), build: %s %s\n^!", ascii_logo[3], __DATE__, __TIME__);   
    kprintf("^p %s ^!\t\t^gUptime^!: %d seconds\n^!", ascii_logo[4], uptime_s);   
    kprintf("^p %s ^!\t\t^gShell^!: ksh\n^!", ascii_logo[5]);   
    kprintf("^p %s ^!\t\t^gDE^!: shitgui\n^!", ascii_logo[6]);   
    kprintf("^p %s ^!\t\t^gCPU^!: %s (^yFamily^!: %d; ^yModel^!: %d)\n^!", ascii_logo[7], g_cpu.vendor, g_cpu.family, g_cpu.model);
    kprintf("\n\n");
}

void cmd_meow() {
    u64 cats_count = sizeof(cats) / sizeof(cats[0]);
    u8 rand_num = krand() % cats_count;
    kprintf("Nyaaa!!\n\n%s\n\n", cats[rand_num]);
}

void cmd_help() {
    kprintf("Welcome to ^ptermOS^!'s ^gk^!ernel ^gsh^!ell!\n");
    kprintf("It loads when userspace is failed to load and acts as a basic rescue environment\n");
    kprintf("Available commands:\n");
    
    kprintf("\t^rDebug^!:\n");
    kprintf("\t\t^ysleep^!       \t\tSleep for 3seconds\n");
    kprintf("\t\t^ydbg^!         \t\tTest new stuff\n");
    kprintf("\t\t^yregs^!        \t\tPrint current regs\n");
    kprintf("\t\t^ypanic^!       \t\tPanics (lol)\n");
    kprintf("\t\t^yud2^!         \t\tPanics with #UD\n");
    kprintf("\t\t^ypf^!          \t\tPanics with #PF\n");
    kprintf("\t\t^yuserspace^!   \t\tAttempt to jump in ring 3\n");
    
    kprintf("\t^pFun^!:\n");
    kprintf("\t\t^ysplash^!      \t\tShows splash (works kinda unstable)\n");
    kprintf("\t\t^ymeow^!        \t\tcats!!!\n");
    kprintf("\t\t^ykfetch^!      \t\tr/unixporn compatible\n");
    
    kprintf("\t^gCustomisation^!:\n");
    kprintf("\t\t^yblinking^!    \t\tDisable/Enable cursor blinking\n");

    kprintf("\t^bMisc^!:\n");
    kprintf("\t\t^yclear^!       \t\tClear console\n");
    kprintf("\t\t^yhelp^!        \t\tShow this menu\n");
    kprintf("\t\t^yrand^!        \t\tGet a random number\n");
    kprintf("\t\t^yver^!         \t\tDisplays termOS's version\n");
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


void cmd_sleep() {
    sleep(3000);
}

void cmd_rand() {
    kprintf("Your rand number (0..100) is ... ^y%d^!!\n", krand() % 100);
}

void cmd_debug() {
    u64 status = debug();
    kprintf("\nDebug ended with code %d\n", status);
    return;
}

void cmd_ver() {
    kprintf("termOS version %s\n", TERMOS_VERSION);
    kprintf("Dewar Kernel (x86_64), build: %s %s\n", __DATE__, __TIME__);
    kprintf("License: GPL-3.0-or-later\n");
}

void cmd_userspace() {
    kprintf("Command disabled due to be really broken\n");
}