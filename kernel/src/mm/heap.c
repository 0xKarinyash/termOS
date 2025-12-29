// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <mm/heap.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/memory.h>
#include <core/panic.h>
#include <types.h>

#define HEAP_SIZE_PAGES 32

extern u64* pml4_kernel;
static block_header* heap_list_head = nullptr;

void combine_forward(block_header* curr) {
    if (!curr->next || !curr->next->is_free) return;
    curr->size += sizeof(block_header) + curr->next->size;
    curr->next = curr->next->next;
    if (curr->next) curr->next->prev = curr; // what the fuck
}

void heap_init() {
    u64 heap_start = KERNEL_HEAP_START;

    for (u64 i = 0; i < HEAP_SIZE_PAGES; i++) {
        u64 phys = (u64)pmm_alloc_page();
        if (!phys) panic("OOM during heap init");

        u64 virt = heap_start + (i * PAGE_SIZE);
        vmm_map_page(pml4_kernel, phys, virt, PTE_PRESENT | PTE_RW);
    }

    heap_list_head = (block_header*)heap_start;
    heap_list_head->magic = HEADER_MAGIC;
    heap_list_head->size = (HEAP_SIZE_PAGES * PAGE_SIZE) - sizeof(block_header);
    heap_list_head->is_free = true;
    heap_list_head->next = nullptr;
    heap_list_head->prev = nullptr;
}

void* malloc(u64 size) {
    if (size == 0) return nullptr;
    u64 aligned_size = (size + 15) & ~15;

    block_header* curr = heap_list_head;
    while (curr) {
        if (curr->is_free && curr->size >= aligned_size) {
            if (curr->size > aligned_size + sizeof(block_header) + 16) {
                block_header* new_block = (block_header*)((u64)curr + sizeof(block_header) + aligned_size);
                new_block->size = curr->size - aligned_size - sizeof(block_header);
                new_block->is_free = true;
                new_block->next = curr->next;
                new_block->prev = curr;
                new_block->magic = HEADER_MAGIC;
                
                if (curr->next) curr->next->prev = new_block;
                curr->next = new_block;
                curr->size = aligned_size;
            }
            curr->is_free = false;
            return (void*)((u64)curr + sizeof(block_header));
        }
        curr = curr->next;
    }

    return nullptr;
}

void free(void* ptr) {
    if (!ptr) return;

    block_header* curr = (block_header*)((u64)ptr - sizeof(block_header));
    if (curr->magic != HEADER_MAGIC) return;
    
    curr->is_free = true;
    if (curr->next && curr->next->is_free) combine_forward(curr);
    if (curr->prev && curr->prev->is_free) combine_forward(curr->prev);
}

void* realloc(void* ptr, u64 new_size) {
    if (!ptr) return malloc(new_size);
    if (new_size == 0) { 
        free(ptr); 
        return nullptr; 
    }

    block_header* curr = (block_header*)((u64)ptr - sizeof(block_header));
    if (curr->size >= new_size) return ptr;

    if (curr->next &&
        curr->next->is_free &&
        (curr->size + sizeof(block_header) + curr->next->size) >= new_size) { // why ts shit so fucking unreadable
            combine_forward(curr);
            return ptr;
    }

    void* new_ptr = malloc(new_size);
    if (!new_ptr) return nullptr;

    memcpy(new_ptr, ptr, curr->size);
    free(ptr);

    return new_ptr;
}