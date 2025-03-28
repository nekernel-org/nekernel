/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @file HeFS.h
/// @brief HeFS filesystem support.

#define kHeFSVersion  (0x0100)
#define kHeFSMagic	  "  HeFS"
#define kHeFSMagicLen (8)

#define kHeFSFileNameLen (256U)
#define kHeFSPartNameLen (256U)

#define kHeFSMinimumDiskSize (mib_cast(256))

enum
{
	kHeFSInvalidDrive,
	kHeFSHDDDrive,
	kHeFSSSDDrive,
	kHeFSMassStorageDrive,
	kHeFSSCSIDrive,
	kHeFSDriveCount,
};

struct HeFS_BOOT_NODE;

struct HeFS_BOOT_NODE final
{
	Kernel::Char	 fMagic[kHeFSMagicLen];
	Kernel::Char	 fPartName[kHeFSPartNameLen];
	Kernel::UInt32 fVersion;
	Kernel::UInt64 fBadSectors;
	Kernel::UInt64 fSectorCount;
	Kernel::UInt64 fSectorSize;
	Kernel::UInt32 fChecksum;
	Kernel::UInt8	 fDriveKind;
	Kernel::UInt8	 fTextEncoding;
	Kernel::UInt64 fRootINode;
	Kernel::UInt64 fRecoveryINode;
};

struct HeFS_INDEX_NODE
{
	Kernel::Char	 fName[kHeFSFileNameLen];
	Kernel::UInt32 fFlags;
	Kernel::UInt16 fKind;
	Kernel::UInt32 fSize;
	Kernel::Lba	 fFirstINode;
	Kernel::Lba	 fLastINode;
	Kernel::UInt32 fChecksum;
};