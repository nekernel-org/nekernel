/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <hint/CompilerHint.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <KernelKit/User.h>
#include <NewKit/Crc32.h>

/// @file HeFS.h
/// @brief HeFS filesystem support.

#define kHeFSVersion  (0x0100)
#define kHeFSMagic	  "  HeFS"
#define kHeFSMagicLen (8)

#define kHeFSFileNameLen (256U)
#define kHeFSPartNameLen (128U)

#define kHeFSMinimumDiskSize (gib_cast(4))

struct HeFS_BOOT_NODE;
struct HeFS_INDEX_NODE;
struct HeFS_INDEX_NODE_DIRECTORY;

enum
{
	kHeFSHardDrive		   = 0xC0, // Hard Drive
	kHeFSSolidStateDrive   = 0xC1, // Solid State Drive
	kHeFSOpticalDrive	   = 0x0C, // Blu-Ray/DVD
	kHeFSMassStorageDevice = 0xCC, // USB
	kHeFSScsiDrive		   = 0xC4, // SCSI Hard Drive
	kHeFSFlashDrive		   = 0xC6,
	kHeFSUnknown		   = 0xFF, // Unknown device.
	kHeFSDriveCount		   = 7,
};

enum
{
	kHeFSStatusUnlocked = 0x18,
	kHeFSStatusLocked,
	kHeFSStatusError,
	kHeFSStatusInvalid,
	kHeFSStatusCount,
};

enum
{
	kHeFSEncodingUTF8 = 0x00,
	kHeFSEncodingUTF16,
	kHeFSEncodingUTF32,
	kHeFSEncodingUTF16BE,
	kHeFSEncodingUTF16LE,
	kHeFSEncodingUTF32BE,
	kHeFSEncodingUTF32LE,
	kHeFSEncodingUTF8BE,
	kHeFSEncodingUTF8LE,
	kHeFSEncodingCount,
};

inline constexpr UInt16 kHeFSFileKindRegular	  = 0x00;
inline constexpr UInt16 kHeFSFileKindDirectory	  = 0x01;
inline constexpr UInt16 kHeFSFileKindBlock		  = 0x02;
inline constexpr UInt16 kHeFSFileKindCharacter	  = 0x03;
inline constexpr UInt16 kHeFSFileKindFIFO		  = 0x04;
inline constexpr UInt16 kHeFSFileKindSocket		  = 0x05;
inline constexpr UInt16 kHeFSFileKindSymbolicLink = 0x06;
inline constexpr UInt16 kHeFSFileKindUnknown	  = 0x07;
inline constexpr UInt16 kHeFSFileKindCount		  = 0x08;

/// @brief HeFS blocks are array containing sparse blocks of data.
/// @details The blocks are used to store the data of a file. Each block is a pointer to a block of data on the disk.
inline constexpr UInt16 kHeFSBlockCount = 0x10;

struct PACKED HeFS_BOOT_NODE final
{
	Kernel::Char	  fMagic[kHeFSMagicLen];
	Kernel::Utf16Char fVolName[kHeFSPartNameLen];
	Kernel::UInt32	  fVersion;
	Kernel::UInt64	  fBadSectors;
	Kernel::UInt64	  fSectorCount;
	Kernel::UInt64	  fSectorSize;
	Kernel::UInt32	  fChecksum;
	Kernel::UInt8	  fDriveKind;
	Kernel::UInt8	  fEncoding;
	Kernel::UInt64	  fStartIND;
	Kernel::UInt64	  fEndIND;
	Kernel::UInt64	  fINodeCount;
	Kernel::UInt64	  fDiskSize;
	Kernel::UInt16	  fDiskStatus;
	Kernel::UInt16	  fDiskFlags;
	Kernel::UInt16	  fVID; // virtual identification number within an EPM disk.
};

/// @brief Access time type.
/// @details Used to keep track of the INode, INodeDir allocation status.
typedef Kernel::UInt64 ATime;

inline constexpr ATime kHeFSTimeInvalid = 0x0000000000000000;
inline constexpr ATime kHeFSTimeMax		= 0xFFFFFFFFFFFFFFFF;

/// @brief HeFS index node.
/// @details This structure is used to store the file information of a file.
/// @note The index node is a special type of INode that contains the file information.
/// @note The index node is used to store the file information of a file.
struct PACKED HeFS_INDEX_NODE final
{
	Kernel::Utf16Char fName[kHeFSFileNameLen];
	Kernel::UInt32	  fFlags;
	Kernel::UInt16	  fKind;
	Kernel::UInt32	  fSize;
	Kernel::UInt32	  fChecksum, fRecoverChecksum, fBlockChecksum, fLinkChecksum;

	ATime		   fCreated, fAccessed, fModified, fDeleted;
	Kernel::UInt32 fUID, fGID;
	Kernel::UInt32 fMode;

	Kernel::UInt64 fBlockLinkStart[kHeFSBlockCount];
	Kernel::UInt64 fBlockLinkEnd[kHeFSBlockCount];

	Kernel::UInt64 fBlockStart[kHeFSBlockCount];
	Kernel::UInt64 fBlockEnd[kHeFSBlockCount];

	Kernel::UInt64 fBlockRecoveryStart[kHeFSBlockCount];
	Kernel::UInt64 fBlockRecoveryEnd[kHeFSBlockCount];

	/// @brief Red-black tree pointers.
	Kernel::Lba fNext, fPrev, fChild, fParent;
};

/// @brief HeFS directory node.
/// @details This structure is used to store the directory information of a file.
/// @note The directory node is a special type of INode that contains the directory entries.
struct PACKED HeFS_INDEX_NODE_DIRECTORY final
{
	Kernel::Utf16Char fName[kHeFSFileNameLen];

	Kernel::UInt32 fFlags;
	Kernel::UInt16 fKind;
	Kernel::UInt32 fSize;
	Kernel::UInt32 fChecksum, fIndexNodeChecksum;

	ATime		   fCreated, fAccessed, fModified, fDeleted;
	Kernel::UInt32 fUID, fGID;
	Kernel::UInt32 fMode;

	Kernel::UInt64 fIndexNodeStart[kHeFSBlockCount];
	Kernel::UInt64 fIndexNodeEnd[kHeFSBlockCount];

	/// @brief Red-black tree pointers.
	Kernel::Lba fNext, fPrev, fChild, fParent;
};

namespace Kernel::Detail
{
	/// @brief HeFS get year from ATime.
	/// @param raw_atime the raw ATime value.
	/// @return the year value.
	/// @note The year is stored in the upper 32 bits of the ATime value.
	inline UInt32 hefs_year_get(ATime raw_atime) noexcept
	{
		return (raw_atime & 0x00000000FFFFFFFF) >> 32;
	}

	/// @brief HeFS get month from ATime.
	/// @param raw_atime the raw ATime value.
	/// @return the month value.
	/// @note The month is stored in the upper 24 bits of the ATime value.
	inline UInt32 hefs_month_get(ATime raw_atime) noexcept
	{
		return (raw_atime & 0x00000000FFFFFFFF) >> 24;
	}

	/// @brief HeFS get day from ATime.
	/// @param raw_atime the raw ATime value.
	/// @return the day value.
	/// @note The day is stored in the upper 16 bits of the ATime value.
	inline UInt32 hefs_day_get(ATime raw_atime) noexcept
	{
		return (raw_atime & 0x00000000FFFFFFFF) >> 16;
	}

	/// @brief HeFS get hour from ATime.
	/// @param raw_atime the raw ATime value.
	/// @return the hour value.
	/// @note The hour is stored in the upper 8 bits of the ATime value.
	inline UInt32 hefs_hour_get(ATime raw_atime) noexcept
	{
		return (raw_atime & 0x00000000FFFFFFFF) >> 8;
	}

	/// @brief HeFS get minute from ATime.
	/// @param raw_atime the raw ATime value.
	/// @return the minute value.
	/// @note The minute is stored in the lower 8 bits of the ATime value.
	inline UInt32 hefs_minute_get(ATime raw_atime) noexcept
	{
		return (raw_atime & 0x00000000FFFFFFFF);
	}

	inline constexpr UInt32 kHeFSBaseYear = 1970;
	inline constexpr UInt32 kHeFSBaseMonth = 1;
	inline constexpr UInt32 kHeFSBaseDay = 1;
	inline constexpr UInt32 kHeFSBaseHour = 0;
	inline constexpr UInt32 kHeFSBaseMinute = 0;
} // namespace Kernel::Detail