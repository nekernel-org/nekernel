/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <DriverKit/KernelPrint.h>

int __ImageStart(void) {
    kernelPrintStr("SampleDriver: Starting up.\n");
    return 0;
}

int __ImageEnd(void) {
    kernelPrintStr("SampleDriver: Shutting down.\n");
    return 0;
}
