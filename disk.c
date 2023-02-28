#include "typedef.h"

void resetDisk(u8 drive) {
    __asm__ (
        "int $0x13":
        :"dl"(drive), "a"(0x00)
    );
}
