/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <DriverKit/KernelString.h>
#include <DriverKit/KernelPrint.h>

#include <Drivers/MahroussUpdate/MahroussUpdate.hxx>

DK_EXTERN int __ImageStart(void) {
    kernelPrintStr("Mahrouss Update: Looking for updates...\r\n");
    UpdateRequest req("mup://release-mahrouss.logic/newos/");

    return 0;
}

DK_EXTERN int __ImageEnd(void) {
    return 0;
}

///! @brief Use this to check your stack, if using MinGW/MSVC.
DK_EXTERN void ___chkstk_ms(void) {}
