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

#define kHeFSMinimumDiskSize (gib_cast(64))

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
	NeOS::Char	 fMagic[kHeFSMagicLen];
	NeOS::Char	 fPartName[kHeFSPartNameLen];
	NeOS::UInt32 fVersion;
	NeOS::UInt64 fBadSectors;
	NeOS::UInt64 fSectorCount;
	NeOS::UInt64 fSectorSize;
	NeOS::UInt32 fChecksum;
	NeOS::UInt8	 fDriveKind;
	NeOS::UInt8	 fTextEncoding;
	NeOS::UInt64 fRootINode;
	NeOS::UInt64 fRecoveryINode;
};

struct HeFS_INDEX_NODE
{
	NeOS::Char	 fName[kHeFSFileNameLen];
	NeOS::UInt32 fFlags;
	NeOS::UInt16 fKind;
	NeOS::UInt32 fSize;
	NeOS::Lba	 fFirstINode;
	NeOS::Lba	 fLastINode;
	NeOS::UInt32 fChecksum;
};