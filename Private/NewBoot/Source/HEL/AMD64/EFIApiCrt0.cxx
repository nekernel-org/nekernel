/*
 *      ========================================================
 *
 *      NewBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include "EFIApi.hxx"

#define main efi_main

typedef EFI_STATUS (*EfiMainType)(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);

EFI_STATUS main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"NewBoot: Booting from EPM...\r\n");

    // init
    newboot_init_epm(ImageHandle);

    //! these two should execute a program if any on it.
    newboot_mount_drive("epm:///system/");

    // bye
    newboot_fini_epm();

    Print(L"NewBoot: No auto-mount found.\r\n");

    return EFI_LOAD_ERROR;
}
