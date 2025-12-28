// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
#include "core/panic.h"
#include <shell/ksh.h>
#include <shell/kfetch.h>
#include <drivers/console.h>
#include <core/string.h>

typedef enum {
    TOKEN_EMPTY,

    TOKEN_NULL,
    TOKEN_ILLEGAL,
    
    TOKEN_HELP,

    TOKEN_KFETCH,

    TOKEN_PANIC,
    TOKEN_PANIC_UD2,
    TOKEN_PANIC_PF,

    TOKEN_QUIT,
    TOKEN_CLEAR
} ksh_token;

typedef struct {
    char* str;
    ksh_token token;
} ksh_command_map;

static const ksh_command_map token_map[] = {
    {"", TOKEN_EMPTY},
    {"q", TOKEN_QUIT},
    {"quit", TOKEN_QUIT},
    {"exit", TOKEN_QUIT},

    {"cls", TOKEN_CLEAR},
    {"clear", TOKEN_CLEAR},
    
    {"kfetch", TOKEN_KFETCH},
    {"fastfetch", TOKEN_KFETCH},
    {"neofetch", TOKEN_KFETCH},
    
    {"panic", TOKEN_PANIC},
    {"ud2", TOKEN_PANIC_UD2},
    {"pf", TOKEN_PANIC_PF},

    {"help", TOKEN_HELP},
    {nullptr, TOKEN_NULL}
};

ksh_token char2token(char* token) {
    for (i32 i = 0; token_map[i].str != nullptr; i++) {
        if (strcmp(token, token_map[i].str) == 0) return token_map[i].token;
    }
    return TOKEN_ILLEGAL;
}

static void print_help() {
    kprintf("\tWelcome to ^ptermOS^0's kernel shell!\n");
    kprintf("\tIt can almost nothing! yet.\n");
}

void ksh() {
    while (true) {
        kprintf("ksh_> ");
        char cmdbuff[256];
        kgets(cmdbuff, 256);
        switch(char2token(cmdbuff)) {
            case TOKEN_EMPTY: continue;

            case TOKEN_QUIT: return; // that'll cause panic lol
            case TOKEN_PANIC: panic("Manually initiated panic");
            case TOKEN_PANIC_UD2: __asm__ volatile ("ud2");
            case TOKEN_PANIC_PF: u64* bad_ptr = (u64*)0xDEADBEEF; *bad_ptr = 666;
            
            case TOKEN_CLEAR: console_clear((u32) console_get_colors() & 0xFFFFFFFF); break;
            case TOKEN_HELP: print_help(); break;
            case TOKEN_KFETCH: kfetch(); break;
            
            default: kprintf("Unknown command!!\n"); break;
        }
    }
}