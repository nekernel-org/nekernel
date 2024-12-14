/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

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

struct HPFS_EXPLICIT_BOOT_SECTOR;
