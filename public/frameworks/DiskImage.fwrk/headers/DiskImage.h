/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: DiskImage.h
	PURPOSE: Disk Imaging.
	
   ------------------------------------------- */

#pragma once

#include <FirmwareKit/EPM.h>
#include <FSKit/NeFS.h>
#include <uuid/uuid.h>
#include <LibSCI/SCI.h>

SInt32 DIFormatDiskToFile(const char* kDiskName		= "Disk",
						  int		  kDiskSectorSz = 512,
						  const int	  kDiskBlockCnt = 1,
						  size_t	  kDiskSz		= gib_cast(4),
						  const char* kOutDisk		= "disk.eimg") noexcept;