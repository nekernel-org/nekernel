/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @file HPFS.h
/// @brief HPFS filesystem support.

#define kHPFSVersion  (0x0100)
#define kHPFSMagic	  "  HPFS"
#define kHPFSMagicLen (8)

#define kHPFSFileNameLen (255)
#define kHPFSPartNameLen (255)

#define kHPFSMinimumDiskSize (gib_cast(64))

enum
{
	kHPFSInvalidDrive,
	kHPFSHDDDrive,
	kHPFSSSDDrive,
	kHPFSMassStorageDrive,
	kHPFSSCSIDrive,
	kHPFSDriveCount,
};

struct HPFS_BOOT_NODE;

struct HPFS_BOOT_NODE final
{
	NeOS::Char	 fMagic[kHPFSMagicLen];
	NeOS::Char	 fPartName[kHPFSPartNameLen];
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

struct HPFS_INDEX_NODE
{
	NeOS::Char	 fName[kHPFSFileNameLen];
	NeOS::UInt32 fFlags;
	NeOS::UInt16 fKind;
	NeOS::UInt32 fSize;
	NeOS::Lba	 fFirstINode;
	NeOS::Lba	 fLastINode;
	NeOS::UInt32 fChecksum;
};