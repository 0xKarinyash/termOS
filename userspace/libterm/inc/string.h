// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

void *memset(void *ptr, int value, usize num);
void* memcpy(void* dest, const void* src, UInt64 n);
Int32 strcmp(const char *s1, const char *s2);
Int32 strncmp(const char* s1, const char* s2, UInt64 n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, UInt64 n);
UInt64 strlen(const char* str);
UInt64 strspn(const char* s, const char* accept);
char* strpbrk(const char* s, const char* accept);
char* strtok(char *s, const char* delim);