// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <types.h>
#include <fs/cpio.h>
#include <fs/vfs.h>
#include <core/string.h>
#include <core/panic.h>
#include <mm/memory.h>
#include <mm/heap.h>
#include <drivers/console.h>

#define ALIGN4(x) (((x) + 3) & ~3)

typedef struct {
    char c_magic[6]; // "070701"
    char c_ino[8];
    char c_mode[8]; // type and flags
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8]; // including \0
    char c_check[8]; // checksum (usually 0)
} cpio_header;

static fs_ops cpio_ops = {
    .read = cpio_read,
    .write = nullptr,
    .open = nullptr,
    .close = nullptr,
};

u64 hex_to_u64(const char* s, i32 len) {
    u64 res = 0;
    for (i32 i = 0; i < len; i++) {
        char c = s[i];
        res <<= 4;
        if (c >= '0' && c <= '9') res += (c - '0');
        else if (c >= 'A' && c <= 'F') res += (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') res += (c - 'a' + 10);
    }

    return res;
}

u64 cpio_read(fs_node* node, u64 offset, u64 size, u8* buff) {
    if (offset > node->len) return 0; // EOF
    if ((offset + size) > node->len) size = node->len - offset;

    memcpy(buff, (char*)node->impl_data + offset, size);
    return size;
}

fs_node* cpio_mount(void* base, u64 size) {
    u8* ptr = (u8*)base;
    u8* end = ptr + size;

    fs_node* root = malloc(sizeof(fs_node));
    if (!root) panic("CPIO: Failed to malloc for root node!");
    memset(root, 0, sizeof(fs_node));
    strcpy(root->name, "/");
    root->flags = FS_DIR;
    root->ops = &cpio_ops;

    fs_node* tail = nullptr;

    while (ptr < end) {
        cpio_header* header = (cpio_header*)ptr;

        if (strncmp(header->c_magic, "070701", 6) != 0) panic("CPIO: Bad magic! Corrupted initramfs");
        u64 namesize = hex_to_u64(header->c_namesize, 8);
        u64 filesize = hex_to_u64(header->c_filesize, 8);

        char* filename = (char*)(ptr + sizeof(cpio_header));
        if (strcmp(filename, "TRAILER!!!") == 0) break;

        u64 header_and_name_len = sizeof(cpio_header) + namesize;
        u64 offset_to_data = ALIGN4(header_and_name_len);
        void* file_content = (void*)(ptr + offset_to_data);

        fs_node* new_node = malloc(sizeof(fs_node));
        if (!new_node) panic("CPIO: Failed to malloc for new node!");
        memset(new_node, 0, sizeof(fs_node));
        strncpy(new_node->name, filename, sizeof(new_node->name) - 1);

        new_node->len = filesize;
        new_node->inode = hex_to_u64(header->c_ino, 8);
        new_node->ops = &cpio_ops;
        new_node->impl_data = file_content;

        u64 mode = hex_to_u64(header->c_mode, 8);
        if ((mode & 0xF000) == 0x4000) new_node->flags = FS_DIR;
        else new_node->flags = FS_FILE;

        if (root->ptr == nullptr) root->ptr = new_node;
        else if (tail) tail->next = new_node;

        tail = new_node;

        kprintf("^bCPIO^!: Found file '^y%s^!' (size ^y%d^!) at ^y%x^!\n", filename, filesize, file_content);

        u64 data_len = ALIGN4(filesize);
        ptr += offset_to_data + data_len;
    }

    return root;
}