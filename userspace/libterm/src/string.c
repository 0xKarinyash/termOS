// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <string.h>

static char* olds;

void *memset(void *ptr, int value, usize num) {
    UInt8 *p = (UInt8 *)ptr;
    while (num--) {
        *p++ = (UInt8)value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* src, UInt64 n) {
    UInt8* d = (UInt8*)dest;
    const UInt8* s = (const UInt8*)src;

    while (n >= 8) {
        *(UInt64*)d = *(const UInt64*)s;
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

Int32 strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

Int32 strncmp(const char* s1, const char* s2, UInt64 n) {
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

char* strncpy(char* dest, const char* src, UInt64 n) {
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

UInt64 strlen(const char* str) {
    UInt64 res = 0;
    for (res = 0; str[res]; res++);
    return res;
}

static inline int is_in_set(char c, const char* set) {
    while (*set) {
        if (*set == c) return 1;
        set++;
    }
    return 0;
}

UInt64 strspn(const char* s, const char* accept) {
    UInt64 count = 0;
    while (*s && is_in_set(*s, accept)) {
        count++;
        s++;
    }
    return count;
}

char* strpbrk(const char* s, const char* accept) {
    while (*s) {
        if (is_in_set(*s, accept)) {
            return (char*)s;
        }
        s++;
    }
    return nullptr;
}
// took from https://github.com/walac/glibc/blob/master/string/strtok.c
char* strtok(char *s, const char* delim) {
    char* token;
    if (s == nullptr) s = olds;
    s += strspn(s, delim);
    if (*s == '\0') {
        olds = s;
        return nullptr;
    }

    token = s;
    s = strpbrk(token, delim);
    if (s == nullptr) olds = token + strlen(token);
    else {
        *s = '\0';
        olds = s + 1;
    }

    return token;
}