// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/Services/OSServiceIO.h>
#include <IO/IOConsole.h>

UInt64 OSServiceWrite(UInt64 fileDescriptor, UInt64 buffer, UInt64 length) {
    if (fileDescriptor == 1 || fileDescriptor == 2) {
        char* string = (char*)buffer;
        for (UInt64 i = 0; i < length; i++) {
            IOConsolePutcharacter(string[i]);
        }
        return length;
    }
    return 0;
}

UInt64 OSServiceRead(UInt64 fileDescriptor, UInt64 buffer, UInt64 count) {
    char* readBuffer = (char*)buffer;
    if (fileDescriptor == 0) {
        for (UInt64 i = 0; i < count; i++) {
            readBuffer[i] = IOConsoleGetCharacter();
        }
        return count;
    }
    return 0;
}