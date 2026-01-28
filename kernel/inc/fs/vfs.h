// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#pragma once
#include <types.h>

#define FS_FILE 0x01
#define FS_DIR 0x02

struct fs_node;
typedef struct fs_node fs_node;

typedef struct {
    u64 (*read)(struct fs_node* node, u64 offset, u64 size, u8* buff);
    u64 (*write)(struct fs_node* node, u64 offset, u64 size, u8* buff);
    void (*open)(struct fs_node* node);
    void (*close)(struct fs_node* node);

    struct fs_node* (*finddir)(struct fs_node* node, char* name);
} fs_ops;

struct fs_node {
    char name[256];
    u32 mask;
    u32 uid;
    u32 gid;
    u32 flags;
    u32 inode;
    u64 len;
    fs_ops* ops;
    void* impl_data;
    struct fs_node* ptr;
    struct fs_node* next;
};

void vfs_init(fs_node* root_node);
u64 vfs_read(fs_node* node, u64 offset, u64 size, u8* buff);
fs_node* vfs_open(const char* path);
void vfs_close(fs_node* node);