// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#pragma once
#include <types.h>
#include <fs/vfs.h>

u64 cpio_read(fs_node* node, u64 offset, u64 size, u8* buff);
fs_node* cpio_mount(void* base, u64 size);