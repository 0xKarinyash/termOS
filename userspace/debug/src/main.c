// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <stdio.h>
#include <malloc.h>

#define VECTOR_BUFFER_SIZE 8

typedef struct {
    i32* data;
    u64 size;
    u64 capacity;
} Vector;

void vector_init(Vector* vec) {
    vec->data = nullptr;
    vec->size = 0;
    vec->capacity = 0;
}

u8 vector_append(Vector* vec, const i32 num) {
    if (vec->size >= vec->capacity) {
        u64 new_cap = (vec->capacity == 0) ? VECTOR_BUFFER_SIZE : vec->capacity * 2;

        i32* new_data = realloc(vec->data, new_cap * sizeof(i32));
        if (!new_data) return 2; // Out of memory

        vec->data = new_data;
        vec->capacity = new_cap;
    }
    vec->data[vec->size] = num;
    vec->size++;

    return 0;
}

void vector_reset(Vector* vec) {
    vec->size = 0;
}

u8 vector_reserve(Vector* vec, u64 new_cap) {
    if (new_cap <= vec->capacity) return 0;
    i32* new_data = realloc(vec->data, new_cap * sizeof(i32));
    if (!new_data) return 2;

    vec->data = new_data;
    vec->capacity = new_cap;

    return 0;
}

void vector_free(Vector* vec) {
    free(vec->data);
    vec->data = nullptr;
    vec->size = vec->capacity = 0;
}

void render_list(Vector* numbers) {
    printf("\nNumbers [%d/%d]: [", numbers->size, numbers->capacity);
    if (numbers->size == 0) printf("]");
    for (u64 i = 0; i < numbers->size; i++) {
        printf("%d", numbers->data[i]);
        if (i + 1 >= numbers->size) {
            printf("]");
            break;
        }
        printf(", ");
    }
}

int main() {
    Vector nums;
    vector_init(&nums);
    printf("Heap test\n");
    printf("press any key to add 1 number press 'q' to exit\n");
    char c = 'd';
    u64 i = 0;
    while (c != 'q') {
        render_list(&nums);
        vector_append(&nums, i);
        i++;
        c = getchar();
    }
    vector_free(&nums);
}