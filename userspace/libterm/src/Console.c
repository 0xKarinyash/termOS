// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <Console.h>
#include <Varargs.h>
#include <String.h>

extern UInt64 OSServiceIORead(UInt64 fileDescriptor, void* buffer, UInt64 length);
extern UInt64 OSServiceIOWrite(UInt64 fileDescriptor, const void* buffer, UInt64 length);

static void sConsoleWriteCharacter(char character) {
    OSServiceIOWrite(1, &character, 1);
}

static inline void sConsoleBufferAdd(char* string, UInt64 size, UInt64* written, char character) {
    if (*written < size - 1 && size > 0) {
        string[*written] = character;
    }
    (*written)++;
}

static Int32 sStringFormatVariadic(char* string, UInt64 size, const char* format, va_list args) {
    UInt64 written = 0;
    for (UInt32 i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            if (format[i] == '\0') break;
            switch (format[i]) {
                case 's': {
                    const char* vaArgString = va_arg(args, const char*);
                    if (!vaArgString) vaArgString = "(null)";
                    while (*vaArgString) sConsoleBufferAdd(string, size, &written, *vaArgString++);
                    break;
                }
                case 'c': {
                    char character = (char)va_arg(args, int);
                    sConsoleBufferAdd(string, size, &written, character);
                    break;
                }
                case 'd': {
                    Int64 number = va_arg(args, int);
                    if (number < 0) {
                        sConsoleBufferAdd(string, size, &written, '-');
                        number = -number;
                    }
                    UInt64 unsignedNumber = (UInt64)number;
                    char tempBuffer[32];
                    Int32 position = 0;
                    if (unsignedNumber == 0) tempBuffer[position++] = '0';
                    while (unsignedNumber > 0) {
                        tempBuffer[position++] = (unsignedNumber % 10) + '0';
                        unsignedNumber /= 10;
                    }

                    while (position > 0) sConsoleBufferAdd(string, size, &written, tempBuffer[--position]);
                    break;
                }
                case 'x':
                case 'X': {
                    UInt64 unsignedNumber = va_arg(args, unsigned long long);
                    UInt8 padding = (format[i] == 'X') ? 16 : 0;

                    char tempBuffer[32];
                    int position = 0;
                    const char* hex = "0123456789ABCDEF";
                    
                    if (unsignedNumber == 0 && padding == 0) tempBuffer[position++] = '0';
                    while (unsignedNumber > 0) {
                        tempBuffer[position++] = hex[unsignedNumber % 16];
                        unsignedNumber /= 16;
                    }

                    while (position < padding) tempBuffer[position++] = '0';
                    while (position > 0) sConsoleBufferAdd(string, size, &written, tempBuffer[--position]);
                    break;
                }
                case '%': {
                    sConsoleBufferAdd(string, size, &written, '%');
                    break;
                }
                default: {
                    sConsoleBufferAdd(string, size, &written, '%');
                    sConsoleBufferAdd(string, size, &written, format[i]);
                    break;
                }
            }
        } else {
            sConsoleBufferAdd(string, size, &written, format[i]);
        }
    }

    if (size > 0) {
        if (written < size) string[written] = '\0';
        else string[size - 1] = '\0';
    }

    return (int)written;
}

Int32 StringFormat(char* destination, usize size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int returnValue = sStringFormatVariadic(destination, size, format, args);
    va_end(args);
    return returnValue;
}

Int32 ConsolePrint(const char *format, ...) {
    char buffer[kConsoleDefaultBufferSize];
    va_list args;
    va_start(args, format);

    int length = sStringFormatVariadic(buffer, sizeof(buffer), format, args);    
    va_end(args);

    UInt64 writeLength = ((UInt64)length < sizeof(buffer)) ? length : (sizeof(buffer) - 1);
    OSServiceIOWrite(1, buffer, writeLength);
    
    return (int)writeLength;
}

Int32 ConsoleGetCharacter() {
    char character;
    UInt64 serviceCallResult = OSServiceIORead(0, &character, 1);
    if (serviceCallResult <= 0) return kConsoleEOF;
    return (int)(unsigned char)character;
}

char* ConsoleReadLine(char* string, UInt64 size) {
    if (size == 0) return string;
    
    UInt64 i = 0;
    int character;
    
    while (i < size - 1) {
        character = ConsoleGetCharacter();
        if (character == kConsoleEOF || character == '\n' || character == '\r') break;
        if (character == '\b') {
            if (i > 0) {
                i--;
                ConsolePrint("\b \b");
            } 
            
            continue;
        }
        
        string[i++] = (char)character;
        
        sConsoleWriteCharacter(character);
    }
    
    string[i] = '\0';
    sConsoleWriteCharacter('\n');
    
    return string;
}