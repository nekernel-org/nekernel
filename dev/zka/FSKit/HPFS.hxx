/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

/// @file HPFS.hxx
/// @brief HPFS filesystem support.

#define kHPFSVersion 0x01000
#define kHPFSMagic "  HPFS"
#define kHPFSMagicLen 8

#define kHPFSMinimumDiskSize (gib_cast(64))

struct HPFS_EXPLICIT_BOOT_SECTOR;
