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

#define PACKED __attribute__((packed))

typedef struct HCFSBTree final {
	wchar_t fCatalogName[255];
	struct {
		uint64_t fDataCatalog;
		uint64_t fRsrcCatalog;
		size_t fDataCatalogSize;
		size_t fRsrcCatalogSize;
	} fCatalogData;
	struct HCFSBTree* fPrevElement;
	struct HCFSBTree* fNextElement;
} PACKED HCFSBTree;