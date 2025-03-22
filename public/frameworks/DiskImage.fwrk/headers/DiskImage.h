/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: DiskImage.h
	PURPOSE: Disk Imaging framework.
	
   ------------------------------------------- */

#pragma once

#include <LibSCI/SCI.h>

#define kDISectorSz			 (512)
#define kDIMinDiskSz		 (1024 * 1024 * 1024)
#define kDIDefaultOutputName "disk.eimg"
#define kDIDefaultDiskName	 "Disk"
#define kDISuccessStatus	 (0)
#define kDIFailureStatus	 (1)

struct DI_DISK_IMAGE;

struct DI_DISK_IMAGE
{
	Char   disk_name[512] = kDIDefaultDiskName;
	SInt32 sector_sz	  = kDISectorSz;
	SInt32 block_cnt	  = 0;
	SizeT  disk_sz		  = kDIMinDiskSz;
	Char   out_name[256]  = kDIDefaultOutputName;
};

/// @brief Format with an EPM partition.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatPartitionEPM(struct DI_DISK_IMAGE& img) noexcept;

/// @brief NeFS format over EPM.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatFilesystemNeFS(struct DI_DISK_IMAGE& img) noexcept;
