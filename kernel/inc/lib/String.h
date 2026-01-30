// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

Int32 strcmp(const char* s1, const char* s2);
Int32 strncmp(const char* s1, const char* s2, UInt64 n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, UInt64 n);
void *memset(void *ptr, int value, Size num);
void* memcpy(void* dest, const void* src, UInt64 n);