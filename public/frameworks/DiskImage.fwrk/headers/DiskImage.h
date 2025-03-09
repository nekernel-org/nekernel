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

SInt32 DIFormatDiskToFile(const char* disk_name = "Disk",
						  int		  sector_sz = 512,
						  const int	  block_cnt = 1,
						  size_t	  disk_sz	= gib_cast(4),
						  const char* out_name	= "disk.eimg") noexcept;