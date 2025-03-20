/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: DiskImage.h
	PURPOSE: Disk Imaging framework.
	
   ------------------------------------------- */

#pragma once

#include <FirmwareKit/EPM.h>
#include <FSKit/NeFS.h>
#include <uuid/uuid.h>
#include <LibSCI/SCI.h>

#define kDISectorSz (512)
#define kDIMinDiskSz gib_cast(1)
#define kDIDefaultOutputName "disk.eimg"

#define kDISuccessStatus 0
#define kDIFailureStatus 1

struct DI_DISK;

struct DI_DISK_IMAGE
{
	char disk_name[512] = "Disk Image";
	int			sector_sz = kDISectorSz;
	const int	block_cnt = 0;
	size_t		disk_sz	  = kDIMinDiskSz;
	char out_name[256]  = kDIDefaultOutputName;
};

/// @brief EPM format disk
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatDisk(struct DI_DISK_IMAGE& img) noexcept;

/// @brief NeFS format over EPM.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatNeFS(struct DI_DISK_IMAGE& img) noexcept;
