// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <IO/IOKeyboard.h>
#include <IO/IOConsole.h>

#include <IO.h>
#include <types.h>

static unsigned char sIOKeyboardMap[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 0x00 - 0x09 */
    '9', '0', '-', '=', '\b', /* Backspace */
    '\t',                 /* Tab */
    'q', 'w', 'e', 'r',   /* 0x10 - 0x13 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 0x1D - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 0x1E - 0x27 */
    '\'', '`',   0,        /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 0x2C - 0x32 */
    'm', ',', '.', '/',   0,              /* Right shift */
    '*',
    0,  /* Alt */
    ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 0x3B - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 0x45 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 0x4F - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

static unsigned char sIOKeyboardMapShifted[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 0x00 - 0x09 */
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 0x10 - 0x13 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,                  /* 0x1D - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 0x1E - 0x27 */
 '"', '~',   0,        /* Left shift (0x2A) */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',            /* 0x2C - 0x32 */
  'M', '<', '>', '?',   0,              /* Right shift (0x36) */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 0x3B - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 0x45 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 0x4F - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys */
};

static Boolean sIOKeyboardIsShiftPressed = false;

IOKeyboardBuffer gIOKeyboardInputBuffer = {0};

void IOKeyboardInterruptsHandler() {
    UInt16 nextHead = (gIOKeyboardInputBuffer.head + 1) % kIOKeyboardBufferSize;

    UInt8 scancode = IOPortRead8(0x60);
    // make code 0x00 - 0x7F; break code = make code + 0x80
    if (scancode < 0x80) {
        unsigned char* keyMap = sIOKeyboardIsShiftPressed ? sIOKeyboardMapShifted : sIOKeyboardMap;
        char ascii = keyMap[scancode];
        if (ascii && nextHead != gIOKeyboardInputBuffer.tail) {
            gIOKeyboardInputBuffer.buffer[gIOKeyboardInputBuffer.head] = ascii;
            gIOKeyboardInputBuffer.head = nextHead;
        } else {
            switch (scancode) {
                case 0x2A: sIOKeyboardIsShiftPressed = true; break;
                default: break;
            }
        }
    } else {
        switch (scancode) {
            case 0xAA: sIOKeyboardIsShiftPressed = false; break;
            default: break;
        }
    }
}
