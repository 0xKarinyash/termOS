// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#include <termOS.h>

#include "parser.h"
#include "tokens.h"

static const char* delim = " \t\r\n";

typedef struct {
    char* str;
    termosh_token token;
} termosh_token_map;

static const termosh_token_map token_map[] = {
    {"q", TOKEN_QUIT},
    {"quit", TOKEN_QUIT},
    {"exit", TOKEN_QUIT},

    {"clear", TOKEN_CLEAR},

    {"help", TOKEN_HELP},
    {"spawn", TOKEN_SPAWN},

    {nullptr, TOKEN_NULL}
};

termosh_token char2token(char* token) {
    for (int i = 0; token_map[i].str != nullptr; i++) {
        if (StringCompare(token, token_map[i].str) == 0) {
            return token_map[i].token;
        }
    }

    return TOKEN_ILLEGAL;
}

int tokenize(char* buffer, char* tokens[], int max_tokens) {
    char* token = StringTokenize(buffer, delim);

    int i = 0;
    while (token != nullptr && i < max_tokens) {
        tokens[i] = token;
        token = StringTokenize(nullptr, delim);
        i++;
    }

    return i;
}
