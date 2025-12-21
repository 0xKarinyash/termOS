#include "panic.h"
#include "console.h"
#include "shitgui.h"

static void hlt_loop() {
    while (1) { 
        __asm__ volatile ("cli; hlt"); 
    }
}


__attribute__((noreturn)) void panic(const char *msg) {
    console_clear(0xFF0000);
    console_set_color(0xFFFFFF);

    kprintf("\n\n");
    kprintf("   KERNEL PANIC    \n");
    kprintf("-------------------\n");
    kprintf("   Reason: %s\n", msg);
    kprintf("-------------------\n");
    kprintf("   System halted.\n");

    hlt_loop();
}   