#include "serial.h"
#include "io.h"

#define PORT 0x3F8 // com1

int serial_init() {
    outb(PORT + 1, 0x00); // interrupt enable register (+1) / disable interruptions
    outb(PORT + 3, 0x80); // line control register (+3) 
    outb(PORT + 0, 0x03); // speed : low byte
    outb(PORT + 1, 0x00); // speed : high byte
    outb(PORT + 3, 0x03); // line control register (+3) / reset DLAB / no parity / 1 stop bit / (8N1)
    outb(PORT + 2, 0xC7); // FIFO Control register (+2) / 1100 0111 / last ones are enable FIFO buff / first ones to clean noise
    outb(PORT + 4, 0x0B); // ready :3
    return 0;
}

static int is_transmit_empty() { 
    return inb(PORT + 5) & 0x20; // checking 5th bit LSR 
}

void serial_writec(char chr) {
    while (is_transmit_empty() == 0); // waiting for buffer to flush (spinlock)
    outb(PORT, chr);
}

void serial_write(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) serial_writec(str[i]);
}