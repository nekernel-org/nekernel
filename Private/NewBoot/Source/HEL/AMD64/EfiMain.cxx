/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include <BootKit/Boot.hxx>
#include <BootKit/Processor.hxx>
#include <BootKit/Protocol.hxx>

extern "C" int EfiMain(void *ImageHandle, EfiSystemTable *SystemTable)
{
    SystemTable->conOut->outputString(SystemTable->conOut, L"HCoreLdr: Starting HCore...\r\n");

    while (true)
    {
        rt_cli();
        rt_halt();
    }

    return 0;
}
