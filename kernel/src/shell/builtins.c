// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 0xKarinyash

#include <types.h>
#include <OSCPU.h>

#include <IO/IOConsole.h>
#include <IO/IOTimer.h>

#include <lib/String.h>
#include <lib/Rand.h>

#include <OS/OSScheduler.h>

#include <VM/VMM.h>
#include <VM/Heap.h>

#include <shell/dbgcmd.h>

#include <Data/cats.h>


const char* asciiLogo[] = {
    "      /\\___/\\     ", 
    "     |  > <  |     ", 
    "     |   w   |     ",
    "     |=======|__   ", 
    "     |       |  |  ", 
    "     |TERMOS |  |  ", 
    "     |       |__|  ",
    "     |_______|     " 
};

void KSHCommandKernelFetch() {
    UInt64 uptimeSeconds = IOTimerGetTicks() / 1000;

    IOConsoleLog("\n\n");
    IOConsoleLog("^p %s ^!\t\t^g kernel^!@^gtermos\n^0", asciiLogo[0]);
    IOConsoleLog("^p %s ^!\t\t^!-------------\n^!", asciiLogo[1]);
    IOConsoleLog("^p %s ^!\t\t^gOS^!: termOS %s\n^!", asciiLogo[2], TERMOS_VERSION);   
    IOConsoleLog("^p %s ^!\t\t^gKernel^!: Dewar (x86_64), build: %s %s\n^!", asciiLogo[3], __DATE__, __TIME__);   
    IOConsoleLog("^p %s ^!\t\t^gUptime^!: %d seconds\n^!", asciiLogo[4], uptimeSeconds);   
    IOConsoleLog("^p %s ^!\t\t^gShell^!: ksh\n^!", asciiLogo[5]);   
    IOConsoleLog("^p %s ^!\t\t^gDE^!: shitgui\n^!", asciiLogo[6]);   
    IOConsoleLog("^p %s ^!\t\t^gCPU^!: %s (^yFamily^!: %d; ^yModel^!: %d)\n^!", asciiLogo[7], gOSBootCPU.vendorID, gOSBootCPU.family, gOSBootCPU.model);
    IOConsoleLog("\n\n");
}

void KSHCommandMeow() {
    UInt64 catsCount = sizeof(cats) / sizeof(cats[0]);
    UInt8 randomNumber = Rand() % catsCount;
    IOConsoleLog("Nyaaa!!\n\n%s\n\n", cats[randomNumber]);
}

void KSHCommandHelp() {
    IOConsoleLog("Welcome to ^ptermOS^!'s ^gk^!ernel ^gsh^!ell!\n");
    IOConsoleLog("It loads when userspace is failed to load and acts as a basic rescue environment\n");
    IOConsoleLog("Available commands:\n");
    
    IOConsoleLog("\t^rDebug^!:\n");
    IOConsoleLog("\t\t^ysleep^!       \t\tSleep for 3seconds\n");
    IOConsoleLog("\t\t^ydbg^!         \t\tTest new stuff\n");
    IOConsoleLog("\t\t^yregs^!        \t\tPrint current regs\n");
    IOConsoleLog("\t\t^ypanic^!       \t\tPanics (lol)\n");
    IOConsoleLog("\t\t^yud2^!         \t\tPanics with #UD\n");
    IOConsoleLog("\t\t^ypf^!          \t\tPanics with #PF\n");

    IOConsoleLog("\t^pFun^!:\n");
    IOConsoleLog("\t\t^ysplash^!      \t\tShows splash (works kinda unstable)\n");
    IOConsoleLog("\t\t^ymeow^!        \t\tcats!!!\n");
    IOConsoleLog("\t\t^ykfetch^!      \t\tr/unixporn compatible\n");
    
    IOConsoleLog("\t^bMisc^!:\n");
    IOConsoleLog("\t\t^yclear^!       \t\tClear console\n");
    IOConsoleLog("\t\t^yhelp^!        \t\tShow this menu\n");
    IOConsoleLog("\t\t^yrand^!        \t\tGet a random number\n");
    IOConsoleLog("\t\t^yver^!         \t\tDisplays termOS's version\n");
}

void KSHCommandRegisters() {
    __asm__ volatile ("int3");
} 

void KSHCommandSleep() {
    OSSchedulerYield(3000);
}

void KSHCommandRand() {
    IOConsoleLog("Your rand number (0..100) is ... ^y%d^!!\n", Rand() % 100);
}

void KSHCommandDebug() {
    UInt64 status = KSHDebug();
    IOConsoleLog("\nDebug ended with code %d\n", status);
    return;
}

void KSHCommandVer() {
    IOConsoleLog("termOS version %s\n", TERMOS_VERSION);
    IOConsoleLog("Dewar Kernel (x86_64), build: %s %s\n", __DATE__, __TIME__);
    IOConsoleLog("License: GPL-3.0-or-later\n");
}

void cmd_userspace() {
    IOConsoleLog("Command disabled due to be really broken\n");
}