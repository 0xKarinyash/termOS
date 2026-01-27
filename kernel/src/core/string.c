// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <core/string.h>

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