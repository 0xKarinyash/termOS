// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#pragma once

#include "tokens.h"

termosh_token char2token(char* token);
int tokenize(char* buffer, char* tokens[], int max_tokens);