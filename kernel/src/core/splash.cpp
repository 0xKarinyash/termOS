// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash
#include <drivers/shitgui.h>
#include <drivers/console.h>
#include "../data/logo.h"

void show_splash(SG_Context *sg_ctx) {
    SG_Point logo_point = {0, 10};
    sg_put_img(sg_ctx, &logo_point, &logo_img);
    SG_Point start_pos = {75, 55}; // greeting
    console_set_cursor_pos(&start_pos);
    kprintf("Welcome to ^ptermOS^!!!!\n");

    SG_Point text_normal_point = {0, 120};  // not nice to hardcode nums like that but we have what we have
    console_set_cursor_pos(&text_normal_point);
}
