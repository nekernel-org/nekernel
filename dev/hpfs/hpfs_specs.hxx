/* -------------------------------------------

	Copyright ZKA Web Services Co.

	FILE: Defines.hxx
	PURPOSE: HPFS IFS defines.

------------------------------------------- */

#pragma once

#include <ddk/ddk.h>

#define kHPFSVersion 0x01000
#define kHPFSMagic "  HPFS"
#define kHPFSMagicLen 8

/** @brief Drive type enum. */
enum
{
	kHPFSInvalidDrive,
	kHPFSCDROMDrive,
	kHPFSHDDDrive,
	kHPFSSSDDrive,
	kHPFSUSBDrive,
};

/** @brief Explicit Boot Sector structure. */
struct HPFS_EXPLICIT_BOOT_SECTOR;
