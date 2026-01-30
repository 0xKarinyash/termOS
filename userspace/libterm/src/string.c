// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <string.h>

void *memset(void *ptr, int value, usize num) {
    u8 *p = (u8 *)ptr;
    while (num--) {
        *p++ = (u8)value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, u64 n) {
    u8* d = (u8*)dest;
    const u8* s = (const u8*)src;

    while (n >= 8) {
        *(u64*)d = *(const u64*)s;
        d += 8;
        s += 8;
        n -= 8;
    }

    while (n > 0) {
        *d++ = *s++;
        n--;
    }

    return dest;
}

i32 strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

i32 strncmp(const char* s1, const char* s2, u64 n) {
    while (n > 0) {
        if (*s1 != *s2) return *(unsigned char*)s1 - *(unsigned char*)s2;
        if (*s1 == '\0') return 0;
        s1++;
        s2++;
        n--;
    }
    
    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* saved = dest;
    while (*src) *dest++ = *src++;
    *dest = 0;
    return saved;
}

char* strncpy(char* dest, const char* src, u64 n) {
    char* saved = dest;
    while (*src && n > 0) {
        *dest++ = *src++;
        n--;
    }
    while (n > 0) {
        *dest++ = 0;
        n--;
    }
    return saved;
}

u64 strlen(const char* str) {
    u64 res = 0;
    for (res = 0; str[res]; res++);
    return res;
}