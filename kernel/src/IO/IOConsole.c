// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <IO/IOConsole.h>
#include <IO/IOSerial.h>
#include <IO/IOGraphics.h>
#include <IO/IOKeyboard.h>

#include <lib/Math.h>
#include <OS/OSScheduler.h>

#include <types.h>

#include <stdarg.h>

#include <Data/VGAFont.h>

static IOGraphicsContext* sIOGraphicsContextPointer = nullptr;
static IOGraphicsPoint sIOConsoleCursorPosition = {0};
static IOGraphicsFont sIOConsoleFont = {0};
static IOGraphicsColor sIOConsoleForegroundColor = kIOConsoleColorWhite;
static IOGraphicsColor sIOConsoleDefaultForegroundColor = kIOConsoleColorWhite;
static IOGraphicsColor sIOConsoleBackgroundColor = kIOConsoleColorBlack;

void IOConsoleInit(IOGraphicsContext *context) {
    sIOGraphicsContextPointer = context;
    sIOConsoleCursorPosition.x = 0;
    sIOConsoleCursorPosition.y = 0;

    sIOConsoleFont.size.height = 16;
    sIOConsoleFont.size.width = 8;
    sIOConsoleFont.baseAddress = (const unsigned char*)VGAFont;
}

UInt64 IOConsoleGetColors() {
    return ((UInt64) sIOConsoleForegroundColor << 32) | sIOConsoleBackgroundColor;
}

void IOConsoleClear(IOGraphicsColor color) {
    if (!sIOGraphicsContextPointer) return;

    IOGraphicsPoint point = {0, 0};
    IOGraphicsDrawRectangle(
        sIOGraphicsContextPointer, 
        &point, 
        &sIOGraphicsContextPointer->dimensions,
        color
    );
    
    sIOConsoleCursorPosition.x = 0;
    sIOConsoleCursorPosition.y = 0;
    sIOConsoleBackgroundColor = color;
}

IOGraphicsContext* IOConsoleGetGraphicsContext() {
    return sIOGraphicsContextPointer;
}

IOGraphicsSize IOConsoleGetDimensions() {
    return sIOGraphicsContextPointer->dimensions;
}

void IOConsoleSetCursorPosition(IOGraphicsPoint* point) {
    if (!point) return;
    sIOConsoleCursorPosition.x = point->x;
    sIOConsoleCursorPosition.y = point->y;
}

void IOConsolePutcharacter(char character) {
    IOSerialWriteChar(character);
    if (!sIOGraphicsContextPointer) return;
    if (character == '\n') {
        sIOConsoleCursorPosition.x = 0;
        sIOConsoleCursorPosition.y += sIOConsoleFont.size.height;
    } else if (character == '\t') {
        sIOConsoleCursorPosition.x += sIOConsoleFont.size.width * 4;
    } else if (character == '\b') {
        sIOConsoleCursorPosition.x -= sIOConsoleFont.size.width;
        IOGraphicsDrawRectangle(
            sIOGraphicsContextPointer, 
            &sIOConsoleCursorPosition, 
            &sIOConsoleFont.size, 
            sIOConsoleBackgroundColor
        );
    } else {
        IOGraphicsDrawCharacter(
            sIOGraphicsContextPointer, 
            &sIOConsoleCursorPosition, 
            character, 
            sIOConsoleForegroundColor, 
            &sIOConsoleFont
        );
        sIOConsoleCursorPosition.x += sIOConsoleFont.size.width;
    }

    if (sIOConsoleCursorPosition.x >= sIOGraphicsContextPointer->dimensions.width) {
        sIOConsoleCursorPosition.x = 0;
        sIOConsoleCursorPosition.y += sIOConsoleFont.size.height;
    }
    if (sIOConsoleCursorPosition.y >= sIOGraphicsContextPointer->dimensions.height) {
        IOConsoleClear(sIOConsoleBackgroundColor); // TODO: do scrolling
    }
}

void IOConsolePrint(const char *string) {
    for (Int32 i = 0; string[i] != '\0'; i++) IOConsolePutcharacter(string[i]);
}

static void sIOConsolePrintDecimal(const Int64 number) {
    if (number < 0) IOConsolePutcharacter('-');
    UInt64 unsignedNumber = abs(number);
    char buffer[32];
    Int32 i = 0;

    do {
        buffer[i] = (unsignedNumber % 10) + '0';
        unsignedNumber /= 10;
        i++;
    } while (unsignedNumber > 0);

    while (--i >= 0) {
        IOConsolePutcharacter(buffer[i]);
    }
}

static void sIOConsolePrintHexadecimal(UInt64 unsignedNumber, UInt8 padding) {
    IOConsolePutcharacter('0');
    IOConsolePutcharacter('x');

    if (unsignedNumber == 0) {
        IOConsolePutcharacter('0');
        for (Int32 i = 1; i < padding; i++) IOConsolePutcharacter('0'); 
        return;
    }

    char buffer[16] = {0}; 
    Int32 i = 0;

    do {
        Int32 digit = unsignedNumber % 16;
        if (digit < 10) buffer[i++] = digit + '0';
        else            buffer[i++] = digit - 10 + 'A';
        unsignedNumber /= 16;
    } while (unsignedNumber > 0);

    while(i < padding) {
        IOConsolePutcharacter('0');
        padding--;
    }

    while (--i >= 0) {
        IOConsolePutcharacter(buffer[i]);
    }
}


void IOConsoleLog(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (Int32 i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    const char *string = va_arg(args, const char*);
                    if (!string) string = "undefined";
                    IOConsolePrint(string);
                    break;
                }
                case 'c': {
                    char character = (char)va_arg(args, int);
                    IOConsolePutcharacter(character);
                    break;
                }
                case 'd': {
                    Int32 number = va_arg(args, Int32);
                    sIOConsolePrintDecimal(number);
                    break;
                }
                case 'x': {
                    UInt64 number = va_arg(args, UInt64);
                    sIOConsolePrintHexadecimal(number, 0);
                    break;    
                }
                case 'X' : {
                    UInt64 number = va_arg(args, UInt64);
                    sIOConsolePrintHexadecimal(number, 16);
                    break;
                }
                default: {
                    IOConsolePutcharacter(fmt[i]);
                    break;
                }
            }
        } else if (fmt[i] == '^') {
            i++;
            switch (fmt[i]) {
                case 'r': IOConsoleSetForegroundColor(kIOConsoleColorRed);       break;
                case 'R': IOConsoleSetForegroundColor(kIOConsoleColorVeryRed);   break;
                case 'g': IOConsoleSetForegroundColor(kIOConsoleColorGreen);     break;
                case 'G': IOConsoleSetForegroundColor(kIOConsoleColorVeryGreen); break;
                case 't': IOConsoleSetForegroundColor(kIOConsoleColorTurqoise);  break;
                case 'b': IOConsoleSetForegroundColor(kIOConsoleColorBlue);      break;
                case 'B': IOConsoleSetForegroundColor(kIOConsoleColorVeryBlue);  break;
                case 'l': IOConsoleSetForegroundColor(kIOConsoleColorLightBlue); break;
                case 'y': IOConsoleSetForegroundColor(kIOConsoleColorYellow);    break;
                case 'c': IOConsoleSetForegroundColor(kIOConsoleColorCyan);      break;
                case 'm': IOConsoleSetForegroundColor(kIOConsoleColorMagneta);   break;
                case 'p': IOConsoleSetForegroundColor(kIOConsoleColorPink);      break;
                case '0': IOConsoleSetForegroundColor(kIOConsoleColorBlack);     break;
                case 'w': IOConsoleSetForegroundColor(kIOConsoleColorWhite);     break;

                case '!': IOConsoleSetForegroundColor(sIOConsoleDefaultForegroundColor); break;
                case '~': IOConsoleSetForegroundColor(~sIOConsoleBackgroundColor);       break;
                
                case '^': IOConsolePutcharacter('^'); break;
                
                default: { 
                    IOConsolePutcharacter('^');
                    IOConsolePutcharacter(fmt[i]);
                    break; 
                }
            }
        } else {
            IOConsolePutcharacter(fmt[i]);
        }
    }

    va_end(args);
}

void IOConsoleSetForegroundColor(UInt32 color) {
    sIOConsoleForegroundColor = color;
}

void IOConsoleSetDefaultForegroundColor(UInt32 color) {
    sIOConsoleDefaultForegroundColor = color;
}

char IOConsoleGetCharacter() {
    while (gIOKeyboardInputBuffer.head == gIOKeyboardInputBuffer.tail) { __asm__ volatile ("sti"); OSSchedulerYield(1); }
    __asm__ volatile ("cli");
    char temp = gIOKeyboardInputBuffer.buffer[gIOKeyboardInputBuffer.tail];
    gIOKeyboardInputBuffer.tail = (gIOKeyboardInputBuffer.tail + 1) % kIOKeyboardBufferSize;
    __asm__ volatile ("sti");
    return temp;
}

void IOConsoleReadLine(char* buffer, UInt32 limit) {
    UInt32 i = 0;
    while (true) {
        char character = IOConsoleGetCharacter();
        switch (character) {
            case '\n': { 
                buffer[i] = '\0';
                IOConsoleLog("\n");
                return;
            }
            case '\b': {
                if (i > 0) {
                    i--;
                    IOConsoleLog("\b \b");
                }
                break;
            }
            default: {
                if (i < limit - 1) { 
                    buffer[i] = character;
                    IOConsoleLog("%c", character);
                    i++;
                }
            }
        }
    }
}