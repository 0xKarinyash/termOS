#include <termOS.h>

int main() {
    for (int i = 0; i < 100; i++) {
        UInt64 pid = ProcessSpawn("/System/CoreServices/debug");
        if (pid < 0) {
            ConsolePrint("[PIDSPAMMER] Error %d\n", pid);
        } else {
            ConsolePrint("[PIDSPAMMER] %d spawned\n", pid);
        }
    }
}