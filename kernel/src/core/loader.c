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

#define USER_STACK_TOP  0x70000000

void init_task_entry() {
    process* init_proc = (process*)malloc(sizeof(process));
    init_proc->pid = 1; 
    init_proc->state = RUNNING; 
    init_proc->pml4_phys = vmm_create_address_space();
    strcpy(init_proc->name, "init");

    fs_node* file = vfs_open("/init");
    if (!file) panic("FATAL: /init not found!");

    u8* file_buffer = (u8*)malloc(file->len);
    vfs_read(file, 0, file->len, file_buffer);

    u64 entry = load_hot(init_proc, file_buffer);
    if (!entry) panic("Invalid HOT executable");
    
    free(file_buffer); 
    vmm_setup_user_stack((u64*)init_proc->pml4_phys);
    sched_spawn((void(*)())entry, init_proc, true, USER_STACK_TOP);

    while(1) { __asm__("sti; hlt"); } 
}