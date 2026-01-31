// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <Types.h>

enum {
    kConsoleEOF = -1,
    kConsoleDefaultBufferSize = 1024
};

Int32 ConsolePrint(const char* format, ...);
Int32 StringFormat(char* destination, UInt64 size, const char* format, ...);
Int32 ConsoleGetCharacter();
char* ConsoleReadLine(char* string, UInt64 size);