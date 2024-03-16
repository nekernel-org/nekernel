/* -------------------------------------------

    Copyright Mahrouss Logic
    all rights reserved.

    File: Disk.hxx

------------------------------------------- */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
};
#endif // __cplusplus

typedef struct __sDISK
{
    FILE* fFile;
    int32_t fDiskFilesystem;
    uintptr_t fDiskCursor;
    size_t fDiskSize;
} DISK, *PDISK;
