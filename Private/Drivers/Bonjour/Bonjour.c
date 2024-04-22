/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <DriverKit/KernelString.h>
#include <DriverKit/KernelPrint.h>

int __ImageStart(void) {
    kernelPrintStr("Bonjour: Starting up zeroconf...\r\n");
    return 0;
}

int __ImageEnd(void) {
    kernelPrintStr("Bonjour: Shutting down zeroconf...\r\n");
    return 0;
}
