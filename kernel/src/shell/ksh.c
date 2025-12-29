// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <shell/ksh.h>
#include <shell/builtins.h>

#include <drivers/console.h>
#include <drivers/shitgui.h>

#include <core/string.h>
#include <core/rand.h>
#include <core/splash.h>
#include <core/panic.h>

typedef enum {
    TOKEN_EMPTY,

    TOKEN_NULL,
    TOKEN_ILLEGAL,
    
    TOKEN_HELP,
    TOKEN_SPLASH,

    TOKEN_KFETCH,
    TOKEN_MEOW,

    TOKEN_SLEEP,

    TOKEN_REGS,
    TOKEN_DEBUG,
    TOKEN_RECTEST,
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

    {"meow", TOKEN_MEOW},
    {"nya", TOKEN_MEOW},
    {"splash", TOKEN_SPLASH},
    {"kfetch", TOKEN_KFETCH},
    {"fastfetch", TOKEN_KFETCH},
    {"neofetch", TOKEN_KFETCH},
    
    {"sleep", TOKEN_SLEEP},
    {"dbg", TOKEN_DEBUG},
    {"regs", TOKEN_REGS},
    {"rectest", TOKEN_RECTEST},
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

void ksh() {
    while (true) {
        kprintf("ksh_> ");
        char cmdbuff[256];
        kgets(cmdbuff, 256);
        switch(char2token(cmdbuff)) {
            case TOKEN_EMPTY: continue;

            case TOKEN_QUIT: return; // that'll cause panic lol

            case TOKEN_SLEEP: cmd_sleep(); break;
            case TOKEN_DEBUG: cmd_debug(); break;
            case TOKEN_REGS: cmd_regs(); break;
            case TOKEN_RECTEST: rectest(0); break;
            case TOKEN_PANIC: panic("Manually initiated panic");
            case TOKEN_PANIC_UD2: __asm__ volatile ("ud2");
            case TOKEN_PANIC_PF: u64* bad_ptr = (u64*)0xDEADBEEF; *bad_ptr = 666;
            
            case TOKEN_SPLASH: show_splash(console_get_context()); break;
            case TOKEN_CLEAR: console_clear((u32) console_get_colors() & 0xFFFFFFFF); break;
            case TOKEN_HELP: cmd_help(); break;
            case TOKEN_KFETCH: cmd_kfetch(); break;
            case TOKEN_MEOW: cmd_meow(); break;
            
            default: kprintf("Unknown command!!\n"); break;
        }
    }
}