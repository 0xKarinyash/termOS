// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <fs/vfs.h>
#include <core/string.h>

fs_node* fs_root = nullptr;

void vfs_init(fs_node* root_node) {
    fs_root = root_node;
}

u64 vfs_read(fs_node* node, u64 offset, u64 size, u8* buff) {
    if (!node) return 0;
    if (!node->ops->read) return 0;

    return node->ops->read(node, offset, size, buff);
}

fs_node* vfs_open(const char* path) {
    if (!fs_root) return nullptr;
    if (strcmp(path, "/") == 0) return fs_root;
    
    const char* search_name = path;
    if (path[0] == '/') search_name++;

    fs_node* curr = fs_root->ptr;
    while (curr != nullptr) {
        if (strcmp(curr->name, search_name) == 0) {
            if (curr->ops->open) curr->ops->open(curr);
            return curr;
        } 
        curr = curr->next;
    }

    return nullptr;
}

void vfs_close(fs_node* node) {
    if (node && node->ops->close) node->ops->close(node);
}