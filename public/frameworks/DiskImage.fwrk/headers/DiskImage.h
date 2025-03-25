/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: DiskImage.h
	PURPOSE: Disk Imaging framework.
	
   ------------------------------------------- */

#pragma once

#include <SCIKit/SystemCalls.h>
#include <NewKit/Defines.h>

#define kDISectorSz			 (512)
#define kDIMinDiskSz		 mib_cast(1)
#define kDIDefaultOutputName "disk.eimg"
#define kDIDefaultDiskName	 "Disk"
#define kDISuccessStatus	 (0)
#define kDIFailureStatus	 (1)

#define kDIDiskNameLen (16)
#define kDIOutNameLen  (256)

/// @brief Disk Image file structure.
/// @param disk_name Disk partition name.
/// @param sector_sz Disk sector_sz.
/// @param block_cnt Disk block count.
/// @param disk_sz Disk size.
/// @param out_name Output file name.
struct DI_DISK_IMAGE
{
	Char   disk_name[kDIDiskNameLen] = kDIDefaultDiskName;
	SInt32 sector_sz				 = kDISectorSz;
	SInt32 block_cnt				 = 0;
	SizeT  disk_sz					 = kDIMinDiskSz;
	Char   out_name[kDIOutNameLen]	 = kDIDefaultOutputName;
};

/// @brief Format with an EPM partition.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatPartitionEPM(struct DI_DISK_IMAGE& img) noexcept;

/// @brief NeFS format over EPM.
/// @param img disk image structure.
/// @return Status code upon completion.
SInt32 DIFormatFilesystemNeFS(struct DI_DISK_IMAGE& img) noexcept;
