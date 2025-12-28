// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

// yes. *fetch in kernel
// WHY FUCKING NOT?? its my OS, my rules

#include <drivers/console.h>

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


void kfetch() {
    char cpu_brand[49];
    //cpu_get_brand_string(cpu_brand);

    kprintf("\n\n");
    kprintf("^p %s ^0\t\t^g kernel^0@^gtermos\n^0", ascii_logo[0]);
    kprintf("^p %s ^0\t\t^0-------------\n^0", ascii_logo[1]);   
    kprintf("^p %s ^0\t\t^gOS^0: termOS 0.0.1\n^0", ascii_logo[2]);   
    kprintf("^p %s ^0\t\t^gKernel^0: sucks\n^0", ascii_logo[3]);   
    kprintf("^p %s ^0\t\t^gUptime^0: 0h 0m\n^0", ascii_logo[4]);   
    kprintf("^p %s ^0\t\t^gShell^0: termosh\n^0", ascii_logo[5]);   
    kprintf("^p %s ^p\t\t^gDE^0: shitgui\n^0", ascii_logo[6]);   
    kprintf("^p %s ^p\t\t^gCPU^0: %s\n^0", ascii_logo[7], cpu_brand);
    kprintf("\n\n");
}