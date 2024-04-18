/* -------------------------------------------

    Copyright Mahrouss Logic

    Purpose: Kernel Text I/O.

------------------------------------------- */

#include "DriverKit/KernelPrint.h"

#ifdef __x86_64__
static void kernelPrintCharAMD64(const char ch) {
    __asm__ volatile("outb %%al, %1" : : "a"(ch), "Nd"(0x3F8) : "memory");
}
#endif // if __x86_64__

DK_EXTERN void kernelPrintChar(const char ch) {
    kernelPrintChar(ch);
}
