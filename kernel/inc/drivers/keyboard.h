#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <types.h>

#define KB_BUFF_SIZE 256
typedef struct { 
    char buffer[KB_BUFF_SIZE];
    u16 head;
    u16 tail;
} kb_buffer;

extern kb_buffer kb_buf;

void kb_handler(Registers *regs);

#endif