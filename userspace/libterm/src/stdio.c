// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <stdio.h>
#include <stdarg.h>
#include <types.h>
#include <string.h>

#define EOF (-1)
#define PRINTF_BUFFER_SIZE 1024

extern u64 sys_read(u64 fd, void* buf, u64 len);
extern u64 sys_write(u64 fd, const void* buf, u64 len);

static void putchar(char c) {
    sys_write(1, &c, 1);
}

static inline void buf_add(char* str, u64 size, u64* written, char c) {
    if (*written < size - 1 && size > 0) {
        str[*written] = c;
    }
    (*written)++;
}

int vsnprintf(char* str, u64 size, const char* fmt, va_list args) {
    u64 written = 0;
    for (u32 i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            if (fmt[i] == '\0') break;
            switch (fmt[i]) {
                case 's': {
                    const char* s = va_arg(args, const char*);
                    if (!s) s = "(null)";
                    while (*s) buf_add(str, size, &written, *s++);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    buf_add(str, size, &written, c);
                    break;
                }
                case 'd': {
                    i64 n = va_arg(args, int);
                    if (n < 0) {
                        buf_add(str, size, &written, '-');
                        n = -n;
                    }
                    u64 u = (u64)n;
                    char tmp[32];
                    i32 pos = 0;
                    if (u == 0) tmp[pos++] = '0';
                    while (u > 0) {
                        tmp[pos++] = (u % 10) + '0';
                        u /= 10;
                    }

                    while (pos > 0) buf_add(str, size, &written, tmp[--pos]);
                    break;
                }
                case 'x':
                case 'X': {
                    u64 u = va_arg(args, unsigned long long);
                    u8 padding = (fmt[i] == 'X') ? 16 : 0;

                    char tmp[32];
                    int pos = 0;
                    const char* hex = "0123456789ABCDEF";
                    
                    if (u == 0 && padding == 0) tmp[pos++] = '0';
                    while (u > 0) {
                        tmp[pos++] = hex[u % 16];
                        u /= 16;
                    }

                    while (pos < padding) tmp[pos++] = '0';
                    while (pos > 0) buf_add(str, size, &written, tmp[--pos]);
                    break;
                }
                case '%': {
                    buf_add(str, size, &written, '%');
                    break;
                }
                default: {
                    buf_add(str, size, &written, '%');
                    buf_add(str, size, &written, fmt[i]);
                    break;
                }
            }
        } else {
            buf_add(str, size, &written, fmt[i]);
        }
    }

    if (size > 0) {
        if (written < size) str[written] = '\0';
        else str[size - 1] = '\0';
    }

    return (int)written;
}

int snprintf(char* str, usize size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(str, size, fmt, args);
    va_end(args);
    return ret;
}

int printf(const char *fmt, ...) {
    char buf[PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);

    int len = vsnprintf(buf, sizeof(buf), fmt, args);    
    va_end(args);

    u64 write_len = ((u64)len < sizeof(buf)) ? len : (sizeof(buf) - 1);
    sys_write(1, buf, write_len);
    
    return (int)write_len;
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
        if (c == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            } 
            
            continue;
        }
        
        str[i++] = (char)c;
        
        putchar(c);
    }
    
    str[i] = '\0';
    putchar('\n');
    
    return str;
}