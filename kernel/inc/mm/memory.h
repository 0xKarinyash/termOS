// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#ifndef MEMORY_H
#define MEMORY_H

#include <types.h>

void* memset(void* ptr, int value, usize num);
void* memcpy(void* dest, const void* src, u64 n);

#endif