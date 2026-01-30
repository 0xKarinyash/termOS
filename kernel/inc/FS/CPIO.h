// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once
#include <types.h>
#include <FS/VFS.h>

UInt64   FSCPIORead(FSVNode* node, UInt64 offset, UInt64 size, UInt8* buffer);
FSVNode* FSCPIOMount(void* baseAddress, UInt64 totalSize);