/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DriveMgr.h>
#include <KernelKit/User.h>
#include <NewKit/Crc32.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <hint/CompilerHint.h>

/// @file HeFS.h
/// @brief HeFS filesystem support.

#define kHeFSVersion (0x0103)
#define kHeFSMagic "  HeFS"
#define kHeFSMagicLen (8)

#define kHeFSBlockLen (512U)
#define kHeFSFileNameLen (256U)
#define kHeFSPartNameLen (128U)

#define kHeFSMinimumDiskSize (gib_cast(128))

#define kHeFSDefaultVolumeName u8"HeFS Volume"

#define kHeFSINDStartOffset (sizeof(HEFS_BOOT_NODE))
#define kHeFSINStartOffset (sizeof(HEFS_INDEX_NODE_DIRECTORY))

#define kHeFSSearchAllStr u8"*"

struct HEFS_BOOT_NODE;
struct HEFS_INDEX_NODE;
struct HEFS_INDEX_NODE_DIRECTORY;
struct HEFS_JOURNAL_NODE;

enum : UInt8 {
  kHeFSHardDrive         = 0xC0,  // Hard Drive
  kHeFSSolidStateDrive   = 0xC1,  // Solid State Drive
  kHeFSOpticalDrive      = 0x0C,  // Blu-Ray/DVD
  kHeFSMassStorageDevice = 0xCC,  // USB
  kHeFSScsiDrive         = 0xC4,  // SCSI Hard Drive
  kHeFSFlashDrive        = 0xC6,
  kHeFSUnknown           = 0xFF,  // Unknown device.
  kHeFSDriveCount        = 7,
};

enum : UInt8 {
  kHeFSStatusUnlocked = 0x18,
  kHeFSStatusLocked,
  kHeFSStatusError,
  kHeFSStatusInvalid,
  kHeFSStatusCount,
};

enum : UInt16 {
  kHeFSEncodingFlagsUTF8 = 0x50,
  kHeFSEncodingFlagsUTF16,
  kHeFSEncodingFlagsUTF32,
  kHeFSEncodingFlagsUTF16BE,
  kHeFSEncodingFlagsUTF16LE,
  kHeFSEncodingFlagsUTF32BE,
  kHeFSEncodingFlagsUTF32LE,
  kHeFSEncodingFlagsUTF8BE,
  kHeFSEncodingFlagsUTF8LE,
  kHeFSEncodingFlagsBinary,
  kHeFSEncodingFlagsCount = 11,
  kHeFSFlagsNone          = 0,
  kHeFSFlagsReadOnly      = 0x100,
  kHeFSFlagsHidden,
  kHeFSFlagsSystem,
  kHeFSFlagsArchive,
  kHeFSFlagsDevice,
  kHeFSFlagsCount = 5
};

inline constexpr UInt16 kHeFSFileKindRegular      = 0x00;
inline constexpr UInt16 kHeFSFileKindDirectory    = 0x01;
inline constexpr UInt16 kHeFSFileKindBlock        = 0x02;
inline constexpr UInt16 kHeFSFileKindCharacter    = 0x03;
inline constexpr UInt16 kHeFSFileKindFIFO         = 0x04;
inline constexpr UInt16 kHeFSFileKindSocket       = 0x05;
inline constexpr UInt16 kHeFSFileKindSymbolicLink = 0x06;
inline constexpr UInt16 kHeFSFileKindUnknown      = 0x07;
inline constexpr UInt16 kHeFSFileKindCount        = 0x08;

/// @brief HeFS blocks are array containing sparse blocks of data.
/// @details The blocks are used to store the data of a file. Each block is a pointer to a block of
/// data on the disk.
inline constexpr UInt16 kHeFSSliceCount = 0x10;

inline constexpr UInt16 kHeFSInvalidVID = 0xFFFF;

namespace Kernel {
/// @brief Access time type.
/// @details Used to keep track of the INode, INodeDir allocation status.
typedef UInt64 ATime;

/// @brief HeFS Boot node.
/// @details Acts like a superblock, it contains the information about the filesystem.
/// @note The boot node is the first block of the filesystem.
struct PACKED HEFS_BOOT_NODE final {
  Char     fMagic[kHeFSMagicLen];       /// @brief Magic number of the filesystem.
  Utf8Char fVolName[kHeFSPartNameLen];  /// @brief Volume name.
  UInt32   fVersion;                    /// @brief Version of the filesystem.
  UInt64   fBadSectors;                 /// @brief Number of bad sectors in the filesystem.
  UInt64   fSectorCount;                /// @brief Number of sectors in the filesystem.
  UInt64   fSectorSize;                 /// @brief Size of the sector.
  UInt32   fChecksum;                   /// @brief Checksum of the boot node.
  UInt8    fDiskKind;  /// @brief Kind of the drive. (Hard Drive, Solid State Drive, Optical
                       /// Drive, etc).
  UInt8  fEncoding;    /// @brief Encoding of the filesystem. (UTF-8, UTF-16, etc).
  UInt64 fStartIND;    /// @brief Start of the INode directory tree.
  UInt64 fEndIND;      /// @brief End of the INode directory tree.
  UInt64 fINDCount;    /// @brief Number of leafs in the INode tree.
  UInt64 fDiskSize;    /// @brief Size of the disk. (Could be a virtual size, that is not the
                       /// real size of the disk.)
  UInt16 fDiskStatus;  /// @brief Status of the disk. (locked, unlocked, error, invalid).
  UInt16 fDiskFlags;   /// @brief Flags of the disk. (read-only, read-write, etc).
  UInt16 fVID;  /// @brief Virtual Identification Number within an EPM disk. (0xFFFF if not used).
  UInt64 fStartIN;     /// @brief Start INodes range
  UInt64 fEndIN;       /// @brief End INodes range
  UInt64 fStartBlock;  /// @brief Start Blocks range
  UInt64 fEndBlock;    /// @brief End Blocks range
  UInt64 fJournalLBA;  /// @brief Boot Node's COW journal LBA.
  Char   fPad[264];
};

inline constexpr ATime kHeFSTimeInvalid = 0x0000000000000000;
inline constexpr ATime kHeFSTimeMax     = 0xFFFFFFFFFFFFFFFF - 1;

/// @brief Journal Node structure
/// @param fHashPath target hash path
/// @param fStatus target status
/// @param fCopyElem copy of element
/// @param fCopyKind kind of element
struct PACKED HEFS_JOURNAL_NODE {
  UInt64 fHashPath;
  UInt64 fStatus;
  UInt64 fCopyElem;
  UInt8  fCopyKind;
  UInt8  fPad[487];
};

/// @brief This enum defines the opcode of the journal, here mentioned as 'kinds'
enum HeFSJournalKind : UInt8 {
  kJournalKindInvalid  = 0x00,
  kJournalKindWrite    = 0x01,
  kJournalKindRename   = 0x02,
  kJournalKindDelete   = 0x03,
  kJournalKindFlagEdit = 0x04,
  kJournalKindCreate   = 0x05,
  kJournalKindCount,
};

/// @brief HeFS index node.
/// @details This structure is used to store the file information of a file.
/// @note The index node is a special type of INode that contains the file information.
/// @note The index node is used to store the file information of a file.
struct PACKED HEFS_INDEX_NODE final {
  UInt64 fHashPath;  /// @brief File name.
  UInt32 fFlags;     /// @brief File flags.
  UInt16 fKind;      /// @brief File kind. (Regular, Directory, Block, Character, FIFO, Socket,
                     /// Symbolic Link, Unknown).
  UInt32 fSize;      /// @brief File size.
  UInt32 fChecksum;  /// @brief Checksum.

  Boolean fSymLink;  /// @brief Is this a symbolic link? (if yes, the fName is the path to
                     /// the file and blocklinkstart and end contains it's inodes.)

  ATime  fCreated, fAccessed, fModified, fDeleted;  /// @brief File timestamps.
  UInt32 fUID, fGID;                                /// @brief User ID and Group ID of the file.
  UInt32 fMode;  /// @brief File mode. (read, write, execute, etc).

  /// @brief Extents system by using blocks
  /// @details Using an offset to ask fBase, and fLength to compute each slice's length.
  UInt32 fOffsetSliceLow;
  UInt32 fOffsetSliceHigh;

  Char fPad[437];
};

enum {
  kHeFSInvalidColor = 0,
  kHeFSRed          = 100,
  kHeFSBlack,
  kHeFSColorCount,
};

/// @brief HeFS directory node.
/// @details This structure is used to store the directory information of a file.
/// @note The directory node is a special type of INode that contains the directory entries.
struct PACKED HEFS_INDEX_NODE_DIRECTORY final {
  UInt64 fHashPath;  /// @brief Directory path as FNV hash.

  UInt32 fFlags;       /// @brief File flags.
  UInt16 fReserved;    /// @note Reserved for future use.
  UInt32 fEntryCount;  /// @brief Entry Count of this directory inode.
  UInt32 fChecksum;    /// @brief Checksum of the file, index node checksum.

  ATime fCreated, fAccessed, fModified,
      fDeleted;       /// @brief File timestamps and allocation status.
  UInt32 fUID, fGID;  /// @brief User ID and Group ID of the file.
  UInt32 fMode;       /// @brief File mode. (read, write, execute, etc).

  /// @note These slices are organized as:
  /// [0] = OFFSET
  /// [1] = SIZE
  /// @note Thus the += 2 when iterating over them.
  UInt64 fINSlices[kHeFSSliceCount];  /// @brief Start of the index node.

  UInt8 fColor;                         /// @brief Color of the node. (Red or Black).
  Lba   fNext, fPrev, fChild, fParent;  /// @brief Red-black tree pointers.

  Char fPad[285];
};
}  // namespace Kernel

namespace Kernel::Detail {
/// @brief HeFS get year from ATime.
/// @param raw_atime the raw ATime value.
/// @return the year value.
/// @note The year is stored in the upper 32 bits of the ATime value.
inline UInt32 hefs_year_get(ATime raw_atime) noexcept {
  return (raw_atime) >> 32;
}

/// @brief HeFS get month from ATime.
/// @param raw_atime the raw ATime value.
/// @return the month value.
/// @note The month is stored in the upper 24 bits of the ATime value.
inline UInt32 hefs_month_get(ATime raw_atime) noexcept {
  return (raw_atime) >> 24;
}

/// @brief HeFS get day from ATime.
/// @param raw_atime the raw ATime value.
/// @return the day value.
/// @note The day is stored in the upper 16 bits of the ATime value.
inline UInt32 hefs_day_get(ATime raw_atime) noexcept {
  return (raw_atime) >> 16;
}

/// @brief HeFS get hour from ATime.
/// @param raw_atime the raw ATime value.
/// @return the hour value.
/// @note The hour is stored in the upper 8 bits of the ATime value.
inline UInt32 hefs_hour_get(ATime raw_atime) noexcept {
  return (raw_atime) >> 8;
}

/// @brief HeFS get minute from ATime.
/// @param raw_atime the raw ATime value.
/// @return the minute value.
/// @note The minute is stored in the lower 8 bits of the ATime value.
inline UInt32 hefs_minute_get(ATime raw_atime) noexcept {
  return (raw_atime) &0xFF;
}

inline constexpr UInt32 kHeFSBaseYear   = 1970;
inline constexpr UInt32 kHeFSBaseMonth  = 1;
inline constexpr UInt32 kHeFSBaseDay    = 1;
inline constexpr UInt32 kHeFSBaseHour   = 0;
inline constexpr UInt32 kHeFSBaseMinute = 0;

inline const Char* hefs_status_to_string(UInt16 status) noexcept {
  switch (status) {
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

inline const Char* hefs_drive_kind_to_string(UInt8 kind) noexcept {
  switch (kind) {
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

inline const Char* hefs_encoding_to_string(UInt8 encoding) noexcept {
  switch (encoding) {
    case kHeFSEncodingFlagsUTF8:
      return "UTF-8";
    case kHeFSEncodingFlagsUTF16:
      return "UTF-16";
    case kHeFSEncodingFlagsUTF32:
      return "UTF-32";
    case kHeFSEncodingFlagsUTF16BE:
      return "UTF-16BE";
    case kHeFSEncodingFlagsUTF16LE:
      return "UTF-16LE";
    case kHeFSEncodingFlagsUTF32BE:
      return "UTF-32BE";
    case kHeFSEncodingFlagsUTF32LE:
      return "UTF-32LE";
    case kHeFSEncodingFlagsUTF8BE:
      return "UTF-8BE";
    case kHeFSEncodingFlagsUTF8LE:
      return "UTF-8LE";
    default:
      return "Unknown";
  }
}

inline const Char* hefs_file_kind_to_string(UInt16 kind) noexcept {
  switch (kind) {
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

inline const Char* hefs_file_flags_to_string(UInt32 flags) noexcept {
  switch (flags) {
    case kHeFSFlagsNone:
      return "No Flags";
    case kHeFSFlagsReadOnly:
      return "Read Only";
    case kHeFSFlagsHidden:
      return "Hidden";
    case kHeFSFlagsSystem:
      return "System";
    case kHeFSFlagsArchive:
      return "Archive";
    case kHeFSFlagsDevice:
      return "Device";
    default:
      return "Unknown";
  }
}
}  // namespace Kernel::Detail

namespace Kernel::HeFS {
/// @brief HeFS filesystem parser class.
/// @details This class is used to parse the HeFS filesystem.
class HeFileSystemParser final {
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
  _Output Bool Format(_Input _Output DriveTrait* drive, _Input const Int32 flags,
                      const Utf8Char* part_name);

  _Output Bool CreateINodeDirectory(_Input DriveTrait* drive, _Input const Int32 flags,
                                    const Utf8Char* dir);

  _Output Bool RemoveINodeDirectory(_Input DriveTrait* drive, _Input const Int32 flags,
                                    const Utf8Char* dir);

  _Output Bool CreateINode(_Input DriveTrait* drive, _Input const Int32 flags, const Utf8Char* dir,
                           const Utf8Char* name, const UInt8 kind);

  _Output Bool DeleteINode(_Input DriveTrait* drive, _Input const Int32 flags, const Utf8Char* dir,
                           const Utf8Char* name, const UInt8 kind);

  _Output Bool INodeManip(_Input DriveTrait* drive, VoidPtr block, SizeT block_sz,
                          const Utf8Char* dir, const Utf8Char* name, const UInt8 kind,
                          const BOOL input);

 private:
  _Output Bool INodeCtlManip(_Input DriveTrait* drive, _Input const Int32 flags,
                             const Utf8Char* dir, const Utf8Char* name, const BOOL delete_or_create,
                             const UInt8 kind);

  _Output Bool INodeDirectoryCtlManip(_Input DriveTrait* drive, _Input const Int32 flags,
                                      const Utf8Char* dir, const BOOL delete_or_create);
};

/// @brief Initialize HeFS inside the main disk.
/// @return Whether it successfuly formated it or not.
Boolean fs_init_hefs(Void);
}  // namespace Kernel::HeFS
