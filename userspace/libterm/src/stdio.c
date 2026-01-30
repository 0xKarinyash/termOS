// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <stdio.h>
#include <stdarg.h>
#include <types.h>
#include <string.h>

#define EOF (-1)

extern u64 sys_read(u64 fd, void* buf, u64 len);
extern u64 sys_write(u64 fd, const void* buf, u64 len);

static void putchar(char c) {
    sys_write(1, &c, 1);
}

static void print_str(const char* str) {
    if (!str) str = "(null)";
    sys_write(1, str, strlen(str));
}

static void print_dec(long long n) {
    if (n < 0) {
        putchar('-');
        n = -n;
    }
    unsigned long long u = (unsigned long long)n;
    char buffer[32];
    int i = 0;
    do {
        buffer[i++] = (u % 10) + '0';
        u /= 10;
    } while (u > 0);
    while (--i >= 0) putchar(buffer[i]);
}

static void print_hex(unsigned long long u, int padding) {
    char buffer[16];
    int i = 0;
    const char* hex_chars = "0123456789ABCDEF";

    do {
        buffer[i++] = hex_chars[u % 16];
        u /= 16;
    } while (u > 0);

    while (i < padding) buffer[i++] = '0';

    print_str("0x");
    while (--i >= 0) putchar(buffer[i]);
}

void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's': print_str(va_arg(args, const char*)); break;
                case 'c': putchar((char)va_arg(args, int));     break;
                case 'd': print_dec(va_arg(args, int));         break;
                case 'x': print_hex(va_arg(args, unsigned long long), 0);  break;
                case 'X': print_hex(va_arg(args, unsigned long long), 16); break;
                case '%': putchar('%'); break;
                default:  putchar(fmt[i]); break;
            }
        }
        else {
            putchar(fmt[i]);
        }
    }

    va_end(args);
}

int getchar() {
    char c;
    unsigned long long res = sys_read(0, &c, 1);
    if (res <= 0) return EOF;
    return (int)(unsigned char)c;
}

char* gets(char* str) {
    int i = 0;
    int c;
    
    while (1) {
        c = getchar();
        
        if (c == EOF || c == '\n' || c == '\r') {
            break;
        }
        
        str[i++] = (char)c;
        
        char ch = (char)c;
        sys_write(1, &ch, 1); 
    }
    
    str[i] = '\0';
    
    char nl = '\n';
    sys_write(1, &nl, 1);
    
    return str;
}

char* gets_s(char* str, u64 size) {
    if (size == 0) return str;
    
    u64 i = 0;
    int c;
    
    while (i < size - 1) {
        c = getchar();
        if (c == EOF || c == '\n' || c == '\r') break;
        
        str[i++] = (char)c;
        
        char ch = (char)c;
        sys_write(1, &ch, 1);
    }
    
    str[i] = '\0';
    char nl = '\n';
    sys_write(1, &nl, 1);
    
    return str;
}