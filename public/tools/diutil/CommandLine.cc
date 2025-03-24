/* -------------------------------------------

	Copyright (C) 2025, Amlal EL Mahrouss, all rights reserved.

	FILE: CommandLine.cc
	PURPOSE: DIUTIL CLI.

   ------------------------------------------- */

#include <DiskImage.fwrk/headers/DiskImage.h>
#include <NewKit/Defines.h>

static const Char	kDiskName[kDIDiskNameLen] = "Disk";
static SInt32		kDiskSectorSz			  = 512;
static const SInt32 kDiskBlockCnt			  = 1;
static SizeT		kDiskSz					  = gib_cast(4);
static const Char	kOutDisk[kDIOutNameLen]	  = "disk.eimg";

/// @brief Filesystem tool entrypoint.
int main(int argc, char** argv)
{
	for (SizeT arg = 0; arg < argc; ++arg)
	{
		const Char* arg_s = argv[arg];

		if (MmCmpMemory((VoidPtr)arg_s, (VoidPtr)"--disk-output-name", MmStrLen("--disk-output-name") == 0))
		{
			if ((arg + 1) < argc)
			{
				MmCopyMemory((VoidPtr)kOutDisk, argv[arg + 1], kDIDiskNameLen);
			}
		}
		else if (MmCmpMemory((VoidPtr)arg_s, (VoidPtr)"--disk-output-size", MmStrLen("--disk-output-size") == 0))
		{
			if ((arg + 1) < argc)
			{
				kDiskSz = MathToNumber(argv[arg + 1], nullptr, 10);
			}
		}
		else if (MmCmpMemory((VoidPtr)arg_s, (VoidPtr)"--disk-sector-size", MmStrLen("--disk-sector-size") == 0))
		{
			if ((arg + 1) < argc)
			{
				kDiskSectorSz = MathToNumber(argv[arg + 1], nullptr, 10);
			}
		}
		else if (MmCmpMemory((VoidPtr)arg_s, (VoidPtr)"--disk-part-name", MmStrLen("--disk-part-name") == 0))
		{
			if ((arg + 1) < argc)
			{
				MmCopyMemory((VoidPtr)kDiskName, argv[arg + 1], kDIDiskNameLen);
			}
		}
	}

	DI_DISK_IMAGE img{};

	return DIFormatPartitionEPM(img);
}