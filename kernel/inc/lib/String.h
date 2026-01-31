// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

void* MemorySet(void* destination, UInt8 value, UInt64 count);
void* MemoryCopy(void* destination, const void* source, UInt64 count);
Int32 StringCompare(const char* firstString, const char* secondString);
Int32 StringCompareWithLimit(const char* firstString, const char* secondString, UInt64 limit);
char* StringCopy(char* destination, const char* source);
char* StringCopyWithLimit(char* destination, const char* source, UInt64 limit);
UInt64 StringGetLength(const char* string);