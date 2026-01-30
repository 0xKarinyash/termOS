// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <IO/IOSerial.h>
#include <IO.h>

enum {
    kIOCom1Port = 0x3F8,
};  

int IOSerialInit() {
    IOPortWrite8(kIOCom1Port + 1, 0x00); // interrupt enable register (+1) / disable interruptions
    IOPortWrite8(kIOCom1Port + 3, 0x80); // line control register (+3) 
    IOPortWrite8(kIOCom1Port + 0, 0x03); // speed : low byte
    IOPortWrite8(kIOCom1Port + 1, 0x00); // speed : high byte
    IOPortWrite8(kIOCom1Port + 3, 0x03); // line control register (+3) / reset DLAB / no parity / 1 stop bit / (8N1)
    IOPortWrite8(kIOCom1Port + 2, 0xC7); // FIFO Control register (+2) / 1100 0111 / last ones are enable FIFO buff / first ones to clean noise
    IOPortWrite8(kIOCom1Port + 4, 0x0B); // ready :3
    return 0;
}

static int IOCheckIsTransmitEmpty() { 
    return IOPortRead8(kIOCom1Port + 5) & 0x20; // checking 5th bit LSR 
}

void IOSerialWriteChar(char character) {
    while (IOCheckIsTransmitEmpty() == 0); // waiting for buffer to flush (spinlock)
    IOPortWrite8(kIOCom1Port, character);
}

void IOSerialWrite(const char* string) {
    for (int i = 0; string[i] != '\0'; i++) IOSerialWriteChar(string[i]);
}