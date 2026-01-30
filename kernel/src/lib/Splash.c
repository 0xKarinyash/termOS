// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash
#include <IO/IOGraphics.h>
#include <IO/IOConsole.h>

#include <Data/logo.h>

void SplashShow(IOGraphicsContext* IOGraphicsContextPointer) {
    IOGraphicsPoint logoPoint = {0, 10};
    IOGraphicsPoint startPoint = {75, 55}; // greeting
    IOGraphicsPoint normalTextPoint = {0, 120}; // not nice to hardcode nums like that but we have what we have
    
    IOGraphicsDrawImage(IOGraphicsContextPointer, &logoPoint, &logoImage);
    IOConsoleSetCursorPosition(&startPoint);
    IOConsoleLog("Welcome to ^ptermOS^!!!!\n");

    IOConsoleSetCursorPosition(&normalTextPoint);
}
