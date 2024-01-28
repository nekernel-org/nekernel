/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include "EFIApi.hxx"

extern "C" int EfiMain(void *ImageHandle, EfiSystemTable *SystemTable)
{
    SystemTable->conOut->output_string(SystemTable->conOut, L"HCORELDR: Preparing Handover structure...\r\n");
    return 0;
}
