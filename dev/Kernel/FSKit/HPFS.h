/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @file HPFS.h
/// @brief HPFS filesystem support.

#define kHPFSVersion  0x0100
#define kHPFSMagic	  "  HPFS"
#define kHPFSMagicLen 8

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