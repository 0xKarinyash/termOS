// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <core/panic.h>
#include <core/hot.h>
#include <core/loader.h>
#include <core/scheduler.h>
#include <core/string.h>

#include <shell/ksh.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/heap.h>
#include <mm/memory.h>

#include <fs/vfs.h>

#include <drivers/console.h>
#include <types.h>

extern task* curr_task;
extern u32 next_pid;

#define USER_STACK_TOP  0x70000000
#define HEAP_START 0x40000000

i32 process_spawn(const char* path, const char* name) {
    fs_node* file = vfs_open(path);
    if (!file) return -1;

    process* new_proc = (process*)malloc(sizeof(process));
    if (!new_proc) return -2;
    memset(new_proc, 0, sizeof(process));
    new_proc->pid = next_pid++; 
    new_proc->state = RUNNING; 
    new_proc->pml4_phys = vmm_create_address_space();
    new_proc->heap_start = HEAP_START;
    new_proc->heap_cur = HEAP_START;
    strncpy(new_proc->name, name, 31);

    u8* file_buffer = (u8*)malloc(file->len);
    if (!file_buffer) {
        free(new_proc);
        return -3;
    }
    vfs_read(file, 0, file->len, file_buffer);

    u64 entry = load_hot(new_proc, file_buffer);
    if (!entry) return -4;

    free(file_buffer);

    vmm_setup_user_stack((u64*)new_proc->pml4_phys);
    sched_spawn((void(*)())entry, new_proc, true, USER_STACK_TOP);

    return new_proc->pid;
}

void init_task_entry() {
    i32 pid = process_spawn("/bin/init", "init");
    if (pid < 0) {
        panic("FATAL: Failed to spawn /bin/init");
    }

    while (1) { __asm__("sti; hlt"); }
}