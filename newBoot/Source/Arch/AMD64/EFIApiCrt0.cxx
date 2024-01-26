/*
 *      ========================================================
 *
 *      newBoot
 *      Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#include "EFIApi.hxx"

#define main efi_main

typedef EFI_STATUS(*EfiMainType)(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);

EFI_STATUS main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) 
{
    InitializeLib(ImageHandle, SystemTable);
    Print(L"newBoot: Booting from XPM...\r\n");

	newboot_init_xpm();
	    
    //! these two should execute a program if any on it.
    newboot_mount_drive("xpm:///system/");
    newboot_mount_drive("xpm:///efi/");

	newboot_fini_xpm();
    
    Print(L"newBoot: No auto-mount found.\r\n");

    return EFI_LOAD_ERROR;
}

