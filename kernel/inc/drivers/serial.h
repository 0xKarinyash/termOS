// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#ifndef SERIAL_H
#define SERIAL_H

int serial_init();
void serial_writec(char chr);
void serial_write(const char *str);

#endif