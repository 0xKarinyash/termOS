// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

int printf(const char *fmt, ...);
int snprintf(char* str, UInt64 size, const char* fmt, ...);
int getchar();
char* gets(char* str);
char* gets_s(char* str, UInt64 size);