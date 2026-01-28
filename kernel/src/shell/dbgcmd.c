// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
// contents of this file will be changed CONSTANTLY
// im just testing new stuff here

#include <drivers/console.h>
#include <shell/dbgcmd.h>
#include <fs/vfs.h>
#include <mm/memory.h>

u64 debug() {
    kprintf("Debug: Trying to open /test1.txt...\n");

    fs_node* file = vfs_open("/test1.txt");

    if (file == nullptr) {
        kprintf("Error: File not found! Check initrd or filename.\n");
        return 1;
    }

    kprintf("Success!\n");
    kprintf("\tFilename: %s\n", file->name);
    kprintf("\tSize: %d bytes\n", file->len);

    char buf[64]; 
    memset(buf, 0, 64);
    vfs_read(file, 0, 63, (u8*)buf);
    kprintf("\tContent: %s\n", buf);

    return 0;
}