// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
// brutally copypasting from kernel/mm/heap.c

#include <malloc.h>
#include <string.h>

static block_header* heap_list_head = nullptr;
extern UInt64 sys_mem(UInt64 size);

void combine_forward(block_header* curr) {
    if (!curr->next || !curr->next->is_free) return;
    curr->size += sizeof(block_header) + curr->next->size;
    curr->next = curr->next->next;
    if (curr->next) curr->next->prev = curr; // what the fuck
}

void* malloc(UInt64 size) {
    if (size == 0) return nullptr;
    UInt64 aligned_size = (size + 15) & ~15;

    block_header* curr = heap_list_head;
    block_header* last = nullptr;
    while (curr) {
        if (curr->is_free && curr->size >= aligned_size) {
            if (curr->size > aligned_size + sizeof(block_header) + 16) {
                block_header* new_block = (block_header*)((UInt64)curr + sizeof(block_header) + aligned_size);
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
            return (void*)((UInt64)curr + sizeof(block_header));
        }
        last = curr;
        curr = curr->next;
    }

    UInt64 need_to_alloc = aligned_size + sizeof(block_header);
    
    UInt64 page_aligned_size = (need_to_alloc + 4095) & ~4095;

    UInt64 new_mem_addr = sys_mem(page_aligned_size);
    if (new_mem_addr == 0) return nullptr; 

    block_header* new_block = (block_header*)new_mem_addr;
    new_block->size = page_aligned_size - sizeof(block_header);
    new_block->is_free = true;
    new_block->magic = HEADER_MAGIC;
    new_block->next = nullptr;
    new_block->prev = last;

    if (last) {
        last->next = new_block;
    } else {
        heap_list_head = new_block;
    }

    return malloc(size);
}

void free(void* ptr) {
    if (!ptr) return;

    block_header* curr = (block_header*)((UInt64)ptr - sizeof(block_header));
    if (curr->magic != HEADER_MAGIC) return;
    
    curr->is_free = true;
    if (curr->next && curr->next->is_free) combine_forward(curr);
    if (curr->prev && curr->prev->is_free) combine_forward(curr->prev);
}

void* realloc(void* ptr, UInt64 new_size) {
    if (!ptr) return malloc(new_size);
    if (new_size == 0) { 
        free(ptr); 
        return nullptr; 
    }

    block_header* curr = (block_header*)((UInt64)ptr - sizeof(block_header));
    if (curr->size >= new_size) return ptr;

    if (curr->next &&
        curr->next->is_free &&
        (curr->size + sizeof(block_header) + curr->next->size) >= new_size) { // why ts so fucking unreadable
            combine_forward(curr);
            return ptr;
    }

    void* new_ptr = malloc(new_size);
    if (!new_ptr) return nullptr;

    memcpy(new_ptr, ptr, curr->size);
    free(ptr);

    return new_ptr;
}