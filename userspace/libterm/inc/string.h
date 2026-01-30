// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

void *memset(void *ptr, int value, usize num);
void* memcpy(void* dest, const void* src, u64 n);
i32 strcmp(const char *s1, const char *s2);
i32 strncmp(const char* s1, const char* s2, u64 n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, u64 n);
u64 strlen(const char* str);