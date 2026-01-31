// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <lib/String.h>

void* MemorySet(void* destination, UInt8 value, UInt64 count) {
    UInt8* bytePointer = (UInt8*) destination;
    while (count--) {
        *bytePointer++ = (UInt8)value;
    }
    return destination;
}

void* MemoryCopy(void* destination, const void* source, UInt64 count) {
    UInt8* destinationBuffer = (UInt8*)destination;
    const UInt8* sourceBuffer = (const UInt8*)source;

    while (count >= 8) {
        *(UInt64*)destinationBuffer = *(const UInt64*)sourceBuffer;
        destinationBuffer += 8;
        sourceBuffer += 8;
        count -= 8;
    }

    while (count > 0) {
        *destinationBuffer++ = *sourceBuffer++;
        count--;
    }

    return destination;
}

Int32 StringCompare(const char* firstString, const char* secondString) {
    while (*firstString && (*firstString == *secondString)) {
        firstString++;
        secondString++;
    }
    return *(const unsigned char*)firstString - *(const unsigned char*)secondString;
}

Int32 StringCompareWithLimit(const char* firstString, const char* secondString, UInt64 limit) {
    while (limit > 0) {
        if (*firstString != *secondString) return *(unsigned char*)firstString - *(unsigned char*)secondString;
        if (*firstString == '\0') return 0;
        firstString++;
        secondString++;
        limit--;
    }
    
    return 0;
}

char* StringCopy(char* destination, const char* source) {
    char* saved = destination;
    while (*source) *destination++ = *source++;
    *destination = 0;
    return saved;
}

char* StringCopyWithLimit(char* destination, const char* source, UInt64 limit) {
    char* saved = destination;
    while (*source && limit > 0) {
        *destination++ = *source++;
        limit--;
    }
    while (limit > 0) {
        *destination++ = 0;
        limit--;
    }
    return saved;
}

UInt64 StringGetLength(const char* string) {
    UInt64 result = 0;
    for (result = 0; string[result]; result++);
    return result;
}