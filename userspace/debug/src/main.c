#include <stdio.h>
#include <process.h>
#include <types.h>

int main() {
    for (int i = 0; i < 100; i++) {
        UInt64 pid = spawn("/System/CoreServices/debug");
        if (pid < 0) {
            printf("[PIDSPAMMER] Error %d\n", pid);
        } else {
            printf("[PIDSPAMMER] %d spawned\n", pid);
        }
    }
}