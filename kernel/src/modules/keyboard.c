#include "keyboard.h"
#include "console.h"
#include "io.h"
#include "types.h"

void kb_handler(Registers *regs) {
    u8 scancode = inb(0x60);
    outb(MASTER_COMMAND, 0x20);
    kprintf("Key: %x\n", scancode);
}