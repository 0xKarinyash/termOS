// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <KSH/KSHEntry.h>
#include <KSH/KSHBuiltins.h>

#include <IO/IOConsole.h>
#include <IO/IOGraphics.h>

#include <OS/OSPanic.h>
#include <OS/OSScheduler.h>

#include <lib/String.h>
#include <lib/Rand.h>
#include <lib/Splash.h>

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

    TOKEN_CLEAR,
    TOKEN_RAND,
    TOKEN_VER,

    TOKEN_BACK,
    TOKEN_FORWARD,
} KSHToken;

typedef struct {
    char* str;
    KSHToken token;
} KSHCommandMap;

static const KSHCommandMap CommandMap[] = {
    {"", TOKEN_EMPTY},    
    // debug
    {"sleep", TOKEN_SLEEP},
    {"dbg", TOKEN_DEBUG},
    {"regs", TOKEN_REGS},
    {"panic", TOKEN_PANIC},
    {"ud2", TOKEN_PANIC_UD2},
    {"pf", TOKEN_PANIC_PF},

    // fun
    {"meow", TOKEN_MEOW},
    {"splash", TOKEN_SPLASH},
    {"kfetch", TOKEN_KFETCH},

    // misc
    {"help", TOKEN_HELP},
    {"clear", TOKEN_CLEAR},
    {"rand", TOKEN_RAND},
    {"ver", TOKEN_VER},

    {nullptr, TOKEN_NULL}
};

KSHToken KSHCharacter2Token(char* token) {
    for (Int32 i = 0; CommandMap[i].str != nullptr; i++) {
        if (StringCompare(token, CommandMap[i].str) == 0) return CommandMap[i].token;
    }
    return TOKEN_ILLEGAL;
}

void KSHEntry() {
    while (true) {
        IOConsoleLog("ksh_> ");
        char cmdbuff[256];
        IOConsoleReadLine(cmdbuff, 256);
        switch(KSHCharacter2Token(cmdbuff)) {
            case TOKEN_EMPTY: continue;

            case TOKEN_CLEAR: IOConsoleClear((UInt32) IOConsoleGetColors() & 0xFFFFFFFF); break;
            case TOKEN_RAND: KSHCommandRand(); break;
            case TOKEN_VER: KSHCommandVer(); break;

            case TOKEN_SLEEP: KSHCommandSleep(); break;
            case TOKEN_DEBUG: KSHCommandDebug(); break;
            case TOKEN_REGS: KSHCommandRegisters(); break;
            case TOKEN_PANIC: OSPanic("Manually initiated panic");
            case TOKEN_PANIC_UD2: __asm__ volatile ("ud2");
            case TOKEN_PANIC_PF: UInt64* bad_ptr = (UInt64*)0xDEADBEEF; *bad_ptr = 666;

            case TOKEN_SPLASH: SplashShow(IOConsoleGetGraphicsContext()); break;
            case TOKEN_KFETCH: KSHCommandKernelFetch(); break;
            case TOKEN_MEOW: KSHCommandMeow(); break;
            
            case TOKEN_HELP: KSHCommandHelp(); break;
            default: IOConsoleLog("Unknown command!!\n"); break;
        }
    }
}