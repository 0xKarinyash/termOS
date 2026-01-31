// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include "tokens.h"
#include "parser.h"
#include "handlers.h"

#include <stdio.h>

#define BUFFER_SIZE 512

int main() {
    while (true) {
        printf("termosh_> ");
        char cmdbuff[BUFFER_SIZE];
        gets_s(cmdbuff, BUFFER_SIZE);
        char* tokens[BUFFER_SIZE];
        int tokens_count = tokenize(cmdbuff, tokens, BUFFER_SIZE);
        if (tokens_count < 1) continue;
        termosh_token token = char2token(tokens[0]);
        switch (token) {
            case TOKEN_QUIT: return 0;
            case TOKEN_CLEAR: not_implemented_yet(); break;
            case TOKEN_HELP: not_implemented_yet(); break;
            case TOKEN_SPAWN: if (tokens_count >= 2) cmd_spawn(tokens[1]); break;
            default: {
                char buff[256];
                snprintf(buff, sizeof(buff), "/System/CoreServices/%s", tokens[0]);
                cmd_spawn(buff);
            }
        }
    }
}