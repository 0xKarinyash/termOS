// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#pragma once
#include <types.h>

extern "C" {
void* memset(void* ptr, int value, usize num);
void* memcpy(void* dest, const void* src, u64 n);
}