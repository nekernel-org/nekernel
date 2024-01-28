/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "EFIApi.hxx"
#include "PartitionMap.hxx"

EFI_FILE_HANDLE kVolume = nullptr;
BootBlockType *kBootFolder = nullptr;
PartitionBlockType *kPartitionFolder = nullptr;

void newboot_init_epm(EFI_HANDLE image)
{
    EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
    EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
    EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
    EFI_FILE_HANDLE Volume;                                 /* the volume's interface */

    /* get the loaded image protocol interface for our "image" */
    uefi_call_wrapper((void *)BS->HandleProtocol, 3, image, &lipGuid, (void **)&loaded_image);
    /* get the volume handle */
    uefi_call_wrapper((void *)BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID *)&IOVolume);
    uefi_call_wrapper((void *)IOVolume->OpenVolume, 2, IOVolume, &Volume);

    kVolume = Volume;

    kBootFolder = (BootBlockType *)AllocatePool(sizeof(BootBlockType));
    kPartitionFolder = (PartitionBlockType *)AllocatePool(sizeof(PartitionBlockType));
}

/**
 * @brief fini the EPM library.
 * Cleans up the memory we claimed and sets the fields to null pointer.
 */
void newboot_fini_epm()
{
    uefi_call_wrapper((void *)kVolume->Close, 1, kVolume);
    kVolume = nullptr;

    FreePool(kBootFolder);
    kBootFolder = nullptr;

    FreePool(kPartitionFolder);
    kPartitionFolder = nullptr;
}
