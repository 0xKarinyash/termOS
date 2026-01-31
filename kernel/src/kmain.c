// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 0xKarinyash

#include "bootinfo.h"

#include <KSH/KSHEntry.h>

#include <types.h>

#include <IO/IOGraphics.h>
#include <IO/IOSerial.h>
#include <IO/IOConsole.h>
#include <IO/IOTimer.h>
#include <IO/IOKeyboard.h>

#include <OS/OSPanic.h>
#include <OS/OSScheduler.h>
#include <OS/Exec/OSLoader.h>

#include <lib/Splash.h>
#include <lib/Rand.h>

#include <GDT.h>
#include <IDT.h>
#include <PIC.h>
#include <OSCPU.h>
#include <syscall.h>

#include <VM/PMM.h>
#include <VM/VMM.h>
#include <VM/Heap.h>

#include <FS/CPIO.h>
#include <FS/VFS.h>

enum {
    kDefaultConsoleForegroundColor = 0xffffff,
    kDefaultConsoleBackgroundColor = 0x111111,
};

extern UInt64 _kernel_end;
extern void* stack_top;
static IOGraphicsContext sIOGraphicsContext;

void kmain(Bootinfo* info) {
    IOSerialInit();
    IOSerialWrite("Kernel started\n");

    IOConsoleInit(&sIOGraphicsContext);

    if (info->magic != BOOTINFO_MAGIC) OSPanic("Corrupt bootinfo!");

    OSCPUInitialize((UInt64)&stack_top);
    IOConsoleLog("Got CPUINFO\n");
    RandInitialize();
    IOConsoleLog("Random initialized\n");
    HALGlobalDescriptorTableInitialize();
    IOConsoleLog("GDT initialized\n");
    HALInterruptsDescriptorTableInitialize();
    IOConsoleLog("IDT initialized\n");
    HALLegacyInterruptControllerRemap();
    IOConsoleLog("PIC remapped\n");
    VMPhysicalMemoryInitialize(&info->memoryMap);
    IOConsoleLog("PMM initialized\n");
    VMVirtualMemoryInitialize(info);
    IOConsoleLog("VMM initialized\n");
    IOTimerInitialize(1000);
    IOConsoleLog("Timer initialized\n");
    VMHeapInitialize();
    IOConsoleLog("Heap initialized\n");
    OSSchedulerInitialize();
    IOConsoleLog("Scheduler initialized\n");

    OSServicesInitialize();
    IOConsoleLog("Syscalls initialized");

    info = (Bootinfo*)PHYS_TO_HHDM((UInt64)info);

    FSVNode* root = FSCPIOMount(PHYS_TO_HHDM(info->initramfs.address), info->initramfs.size);
    FSVirtualFileSystemInitialize(root);
    IOConsoleLog("VFS initialized\n");

    UInt32 *framebuffer = (UInt32*)info->framebuffer.base;
    if (!framebuffer) return OSPanic("No framebuffer found!!");

    sIOGraphicsContext.framebuffer = framebuffer;
    sIOGraphicsContext.dimensions.height = info->framebuffer.height;
    sIOGraphicsContext.dimensions.width  = info->framebuffer.width;
    sIOGraphicsContext.pixelsPerScanLine = info->framebuffer.pitch;

    IOConsoleClear(kDefaultConsoleBackgroundColor);
    IOConsoleSetForegroundColor(kDefaultConsoleForegroundColor);
    IOConsoleSetDefaultForegroundColor(kDefaultConsoleForegroundColor);

    SplashShow(&sIOGraphicsContext);

    bool rescueMode = false;
    if (!info->initramfs.address) { 
        IOConsoleLog("^rWARNING^!: Failed to load ^yStartupVolume^!! Staying in kernel rescue shell!\n\n"); 
        rescueMode = true; 
    }

    if (!rescueMode) {
        IOConsoleLog("Press any key to continue booting. \nPress ^yq^! to stay in ^gksh^!\n");
        char c = '\n';
        c = IOKeyboardGetCharacter();
        if (c == 'q') rescueMode = true;
    }
    
    if (rescueMode) OSSchedulerSpawn(KSHEntry, nullptr, false, 0);
    else OSSchedulerSpawn(init_task_entry, nullptr, false, 0);
    
    __asm__ volatile("sti");
    
    while (true) __asm__ volatile("hlt");
    OSPanic("How in the name of God you got here?");
}