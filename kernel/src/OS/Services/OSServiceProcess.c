// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include <OS/Services/OSServiceProcess.h>
#include <OS/OSScheduler.h>
#include <OS/OSPanic.h>
#include <OS/Exec/OSLoader.h>
#include <IO/IOConsole.h>

Int32 OSServiceProcessExit(Int32 code) {
    IOConsoleLog("\n[Dewar] process \"%s\" exited with code %d\n", gOSSchedulerCurrentTask->process->name, code);
    OSSchedulerTerminate();
    return code;
}


Int32 OSServiceProcessSpawn(const char* path) {
    return OSLoaderProcessSpawn(path, path);
}

Int32 OSServiceProcessWait(UInt64 processID) {
    OSSchedulerBlock(processID);
    return processID;
}
