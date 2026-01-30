// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <IO/IOGraphics.h>

enum {
    kIOConsoleColorRed       = 0xFF5555,
    kIOConsoleColorVeryRed   = 0xFF0000,
    kIOConsoleColorGreen     = 0x08bf39,
    kIOConsoleColorVeryGreen = 0x00FF00,
    kIOConsoleColorTurqoise  = 0x5effaf,
    kIOConsoleColorBlue      = 0x5555FF,
    kIOConsoleColorVeryBlue  = 0x0000FF,
    kIOConsoleColorLightBlue = 0x3890e8,
    kIOConsoleColorYellow    = 0xFFFF55,
    kIOConsoleColorCyan      = 0x55FFFF,
    kIOConsoleColorMagneta   = 0xFF55FF,
    kIOConsoleColorBlack     = 0x000000,
    kIOConsoleColorWhite     = 0xFFFFFF,
    kIOConsoleColorPink      = 0xFFA3B1,
};

void IOConsoleInit(IOGraphicsContext* context);
UInt64 IOConsoleGetColors();
void IOConsoleClear(IOGraphicsColor color);
IOGraphicsContext* IOConsoleGetGraphicsContext();
IOGraphicsSize IOConsoleGetDimensions();
void IOConsoleSetForegroundColor(IOGraphicsColor color);
void IOConsoleSetDefaultForegroundColor(UInt32 color);
void IOConsoleSetCursorPosition(IOGraphicsPoint* point);
char IOConsoleGetCharacter();
void IOConsoleReadLine(char* buffer, UInt32 limit);
void IOConsolePutcharacter(char character);
void IOConsolePrint(const char *string);
void IOConsoleLog(const char *fmt, ...);