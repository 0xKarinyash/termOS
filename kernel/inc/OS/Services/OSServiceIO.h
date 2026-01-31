// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>

UInt64 OSServiceIOWrite(UInt64 fileDescriptor, UInt64 buffer, UInt64 length);
UInt64 OSServiceIORead(UInt64 fileDescriptor, UInt64 buffer, UInt64 count);