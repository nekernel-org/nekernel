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

#define kHeFSVersion  (0x0101)
#define kHeFSMagic	  "  HeFS"
#define kHeFSMagicLen (8)

#define kHeFSFileNameLen (256U)
#define kHeFSPartNameLen (128U)

#define kHeFSMinimumDiskSize (gib_cast(4))

#define kHeFSDefaultVoluneName "HeFS Volume"

struct HEFS_BOOT_NODE;
struct HEFS_INDEX_NODE;
struct HEFS_INDEX_NODE_DIRECTORY;

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

inline constexpr UInt16 kHeFSInvalidVID = 0xFFFF;

namespace Kernel
{
	/// @brief Access time type.
	/// @details Used to keep track of the INode, INodeDir allocation status.
	typedef UInt64 ATime;
} // namespace Kernel

/// @brief HeFS Boot node.
/// @details Acts like a superblock, it contains the information about the filesystem.
/// @note The boot node is the first block of the filesystem.
struct PACKED HEFS_BOOT_NODE final
{
	Kernel::Char	  fMagic[kHeFSMagicLen];	  /// @brief Magic number of the filesystem.
	Kernel::Utf16Char fVolName[kHeFSPartNameLen]; /// @brief Volume name.
	Kernel::UInt32	  fVersion;					  /// @brief Version of the filesystem.
	Kernel::UInt64	  fBadSectors;				  /// @brief Number of bad sectors in the filesystem.
	Kernel::UInt64	  fSectorCount;				  /// @brief Number of sectors in the filesystem.
	Kernel::UInt64	  fSectorSize;				  /// @brief Size of the sector.
	Kernel::UInt32	  fChecksum;				  /// @brief Checksum of the boot node.
	Kernel::UInt8	  fDriveKind;				  /// @brief Kind of the drive. (Hard Drive, Solid State Drive, Optical Drive, etc).
	Kernel::UInt8	  fEncoding;				  /// @brief Encoding of the filesystem. (UTF-8, UTF-16, etc).
	Kernel::UInt64	  fStartIND;				  /// @brief Start of the INode tree.
	Kernel::UInt64	  fEndIND;					  /// @brief End of the INode tree.
	Kernel::UInt64	  fINDCount;				  /// @brief Number of leafs in the INode tree.
	Kernel::UInt64	  fDiskSize;				  /// @brief Size of the disk. (Could be a virtual size, that is not the real size of the disk.)
	Kernel::UInt16	  fDiskStatus;				  /// @brief Status of the disk. (locked, unlocked, error, invalid).
	Kernel::UInt16	  fDiskFlags;				  /// @brief Flags of the disk. (read-only, read-write, etc).
	Kernel::UInt16	  fVID;						  /// @brief Virtual Identification Number within an EPM disk. (0xFFFF if not used).
	Kernel::UInt64	  fReserved;				  /// @brief Reserved for future use.
	Kernel::UInt64	  fReserved2;				  /// @brief Reserved for future use.
	Kernel::UInt64	  fReserved3;				  /// @brief Reserved for future use.
	Kernel::UInt64	  fReserved4;				  /// @brief Reserved for future use.
};

inline constexpr Kernel::ATime kHeFSTimeInvalid = 0x0000000000000000;
inline constexpr Kernel::ATime kHeFSTimeMax		= 0xFFFFFFFFFFFFFFFF;

/// @brief HeFS index node.
/// @details This structure is used to store the file information of a file.
/// @note The index node is a special type of INode that contains the file information.
/// @note The index node is used to store the file information of a file.
struct PACKED ALIGN(8) HEFS_INDEX_NODE final
{
	Kernel::Utf16Char fName[kHeFSFileNameLen];									  /// @brief File name.
	Kernel::UInt32	  fFlags;													  /// @brief File flags.
	Kernel::UInt16	  fKind;													  /// @brief File kind. (Regular, Directory, Block, Character, FIFO, Socket, Symbolic Link, Unknown).
	Kernel::UInt32	  fSize;													  /// @brief File size.
	Kernel::UInt32	  fChecksum, fRecoverChecksum, fBlockChecksum, fLinkChecksum; /// @brief Checksum of the file, recovery checksum, block checksum, link checksum.

	Kernel::ATime  fCreated, fAccessed, fModified, fDeleted; /// @brief File timestamps.
	Kernel::UInt32 fUID, fGID;								 /// @brief User ID and Group ID of the file.
	Kernel::UInt32 fMode;									 /// @brief File mode. (read, write, execute, etc).

	Kernel::UInt64 fBlockLinkStart[kHeFSBlockCount]; /// @brief Start of the block link.
	Kernel::UInt64 fBlockLinkEnd[kHeFSBlockCount];	 /// @brief End of the block link.

	Kernel::UInt64 fBlockStart[kHeFSBlockCount]; /// @brief Start of the block.
	Kernel::UInt64 fBlockEnd[kHeFSBlockCount];	 /// @brief End of the block.

	Kernel::UInt64 fBlockRecoveryStart[kHeFSBlockCount]; /// @brief Start of the block recovery.
	Kernel::UInt64 fBlockRecoveryEnd[kHeFSBlockCount];	 /// @brief End of the block recovery.
};

enum
{
	kHeFSRed = 100,
	kHeFSBlack,
	kHeFSColorCount,
};

/// @brief HeFS directory node.
/// @details This structure is used to store the directory information of a file.
/// @note The directory node is a special type of INode that contains the directory entries.
struct PACKED ALIGN(8) HEFS_INDEX_NODE_DIRECTORY final
{
	Kernel::Utf16Char fName[kHeFSFileNameLen]; /// @brief Directory name.

	Kernel::UInt32 fFlags;						  /// @brief File flags.
	Kernel::UInt16 fKind;						  /// @brief File kind. (Regular, Directory, Block, Character, FIFO, Socket, Symbolic Link, Unknown).
	Kernel::UInt32 fSize;						  /// @brief Size of the directory.
	Kernel::UInt32 fChecksum, fIndexNodeChecksum; /// @brief Checksum of the file, index node checksum.

	Kernel::ATime  fCreated, fAccessed, fModified, fDeleted; /// @brief File timestamps.
	Kernel::UInt32 fUID, fGID;								 /// @brief User ID and Group ID of the file.
	Kernel::UInt32 fMode;									 /// @brief File mode. (read, write, execute, etc).

	/// @note These slices are organized as:
	/// [0] = OFFSET
	/// [1] = SIZE
	/// @note Thus the += 2 when iterating over them.
	Kernel::UInt64 fIndexNodeStart[kHeFSBlockCount]; /// @brief Start of the index node.
	Kernel::UInt64 fIndexNodeEnd[kHeFSBlockCount];	 /// @brief End of the index node.

	Kernel::UInt8 fColor;						 /// @brief Color of the node. (Red or Black).
	Kernel::Lba	  fNext, fPrev, fChild, fParent; /// @brief Red-black tree pointers.
};

namespace Kernel::Detail
{
	/// @brief HeFS get year from Kernel::ATime.
	/// @param raw_atime the raw Kernel::ATime value.
	/// @return the year value.
	/// @note The year is stored in the upper 32 bits of the Kernel::ATime value.
	inline UInt32 hefs_year_get(Kernel::ATime raw_atime) noexcept
	{
		return (raw_atime) >> 32;
	}

	/// @brief HeFS get month from Kernel::ATime.
	/// @param raw_atime the raw Kernel::ATime value.
	/// @return the month value.
	/// @note The month is stored in the upper 24 bits of the Kernel::ATime value.
	inline UInt32 hefs_month_get(Kernel::ATime raw_atime) noexcept
	{
		return (raw_atime) >> 24;
	}

	/// @brief HeFS get day from Kernel::ATime.
	/// @param raw_atime the raw Kernel::ATime value.
	/// @return the day value.
	/// @note The day is stored in the upper 16 bits of the Kernel::ATime value.
	inline UInt32 hefs_day_get(Kernel::ATime raw_atime) noexcept
	{
		return (raw_atime) >> 16;
	}

	/// @brief HeFS get hour from Kernel::ATime.
	/// @param raw_atime the raw Kernel::ATime value.
	/// @return the hour value.
	/// @note The hour is stored in the upper 8 bits of the Kernel::ATime value.
	inline UInt32 hefs_hour_get(Kernel::ATime raw_atime) noexcept
	{
		return (raw_atime) >> 8;
	}

	/// @brief HeFS get minute from Kernel::ATime.
	/// @param raw_atime the raw Kernel::ATime value.
	/// @return the minute value.
	/// @note The minute is stored in the lower 8 bits of the Kernel::ATime value.
	inline UInt32 hefs_minute_get(Kernel::ATime raw_atime) noexcept
	{
		return (raw_atime)&0xFF;
	}

	inline constexpr UInt32 kHeFSBaseYear	= 1970;
	inline constexpr UInt32 kHeFSBaseMonth	= 1;
	inline constexpr UInt32 kHeFSBaseDay	= 1;
	inline constexpr UInt32 kHeFSBaseHour	= 0;
	inline constexpr UInt32 kHeFSBaseMinute = 0;

	inline const Char* hefs_status_to_string(UInt16 status) noexcept
	{
		switch (status)
		{
		case kHeFSStatusUnlocked:
			return "Unlocked";
		case kHeFSStatusLocked:
			return "Locked";
		case kHeFSStatusError:
			return "Error";
		case kHeFSStatusInvalid:
			return "Invalid";
		default:
			return "Unknown";
		}
	}

	inline const Char* hefs_drive_kind_to_string(UInt8 kind) noexcept
	{
		switch (kind)
		{
		case kHeFSHardDrive:
			return "Hard Drive";
		case kHeFSSolidStateDrive:
			return "Solid State Drive";
		case kHeFSOpticalDrive:
			return "Optical Drive";
		case kHeFSMassStorageDevice:
			return "Mass Storage Device";
		case kHeFSScsiDrive:
			return "SCSI/SAS Drive";
		case kHeFSFlashDrive:
			return "Flash Drive";
		case kHeFSUnknown:
		default:
			return "Unknown";
		}
	}

	inline const Char* hefs_encoding_to_string(UInt8 encoding) noexcept
	{
		switch (encoding)
		{
		case kHeFSEncodingUTF8:
			return "UTF-8";
		case kHeFSEncodingUTF16:
			return "UTF-16";
		case kHeFSEncodingUTF32:
			return "UTF-32";
		case kHeFSEncodingUTF16BE:
			return "UTF-16BE";
		case kHeFSEncodingUTF16LE:
			return "UTF-16LE";
		case kHeFSEncodingUTF32BE:
			return "UTF-32BE";
		case kHeFSEncodingUTF32LE:
			return "UTF-32LE";
		case kHeFSEncodingUTF8BE:
			return "UTF-8BE";
		case kHeFSEncodingUTF8LE:
			return "UTF-8LE";
		default:
			return "Unknown";
		}
	}

	inline const Char* hefs_file_kind_to_string(UInt16 kind) noexcept
	{
		switch (kind)
		{
		case kHeFSFileKindRegular:
			return "Regular File";
		case kHeFSFileKindDirectory:
			return "Directory";
		case kHeFSFileKindBlock:
			return "Block Device";
		case kHeFSFileKindCharacter:
			return "Character Device";
		case kHeFSFileKindFIFO:
			return "FIFO";
		case kHeFSFileKindSocket:
			return "Socket";
		case kHeFSFileKindSymbolicLink:
			return "Symbolic Link";
		case kHeFSFileKindUnknown:
		default:
			return "Unknown";
		}
	}

	inline const Char* hefs_file_flags_to_string(UInt32 flags) noexcept
	{
		switch (flags)
		{
		case 0x00:
			return "No Flags";
		case 0x01:
			return "Read Only";
		case 0x02:
			return "Hidden";
		case 0x04:
			return "System";
		case 0x08:
			return "Archive";
		case 0x10:
			return "Device";
		default:
			return "Unknown";
		}
	}

	inline SizeT hefs_get_block_size(SizeT block_size) noexcept
	{
		return block_size * kHeFSBlockCount;
	}
} // namespace Kernel::Detail

namespace Kernel
{
	/// @brief HeFS filesystem parser class.
	/// @details This class is used to parse the HeFS filesystem.
	class HeFileSystemParser final
	{
	public:
		HeFileSystemParser()  = default;
		~HeFileSystemParser() = default;

	public:
		HeFileSystemParser(const HeFileSystemParser&) = delete;
		HeFileSystemParser& operator=(const HeFileSystemParser&) = delete;

		HeFileSystemParser(HeFileSystemParser&&) = delete;
		HeFileSystemParser& operator=(HeFileSystemParser&&) = delete;

	public:
		/// @brief Make a EPM+HeFS drive out of the disk.
		/// @param drive The drive to write on.
		/// @return If it was sucessful, see err_local_get().
		_Output Bool FormatEPM(_Input _Output DriveTrait* drive, _Input const Lba end_lba, _Input const Int32 flags, const Char* part_name);

		/// @brief Make a GPT+HeFS drive out of the disk.
		/// @param drive The drive to write on.
		/// @return If it was sucessful, see err_local_get().
		_Output Bool FormatGPT(_Input _Output DriveTrait* drive, _Input const Lba end_lba, _Input const Int32 flags, const Char* part_name);

	public:
		UInt32 mDriveIndex{MountpointInterface::kDriveIndexA}; /// @brief The drive index which this filesystem is mounted on.
	};
} // namespace Kernel