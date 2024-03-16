/* -------------------------------------------

    Copyright Mahrouss Logic

    File: NewFS.hxx
    Purpose:

    Revision History:

    ?/?/?: Added file (amlel)
    12/02/24: Add UUID macro for EPM and GPT partition schemes.
    3/16/24: Add mandatory sector size, kNewFSMinimumSectorSz is set to 2048 by default.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <KernelKit/DriveManager.hpp>
#include <NewKit/Defines.hpp>

/**
    @brief HCore File System implementation.
*/

#define kNewFSInvalidFork -1
#define kNewFSInvalidCatalog -1
#define kNewFSNodeNameLen 256

#define kNewFSMinimumSectorSz 2048

#define kNewFSIdentLen 8
#define kNewFSIdent "  NEFS"
#define kNewFSPadLen 16

//! On EPM and GPT disks.
#define kNewFSUUID "@{DD997393-9CCE-4288-A8D5-C0FDE3908DBE}"

#define kNewFSVersionInteger 0x121
#define kNewFSVerionString   "1.2.1"

typedef HCore::Char NewCharType;

enum {
  kNewFSHardDrive = 0xC0,          // Hard Drive
  kNewFSOpticalDrive = 0x0C,       // Blu-Ray/DVD
  kNewFSMassStorageDevice = 0xCC,  // USB
  kNewFSUnknown = 0xFF,            // unknown device or unsupported (floppy)
  kNewFSDriveCount = 4,
};

/// @brief NewFS filesystem block.
/// @author Amlal El Mahrouss.
struct PACKED NewBootBlock final {
  NewCharType Ident[kNewFSIdentLen];
  NewCharType Shell[kNewFSNodeNameLen];

  HCore::Int64 NumParts; // number of sub-partitions.
  HCore::Int64 FreeSectors;
  HCore::Int64 SectorCount;
  HCore::Int64 SectorSz;

  HCore::Int64 DiskSize; // size of media.
  HCore::Int32 DiskKind; // kind of disk.

  HCore::Lba FirstPartBlock;
  HCore::Lba LastPartBlock;

  NewCharType Pad[kNewFSPadLen];
};

#define kFlagDeleted 0xF0
#define kFlagUnallocated 0x0F
#define kFlagCatalog 0xFF

#define kKindCatalog 1
#define kKindDirectory 2
#define kKindSymlink 3
#define kKindPartition 4
#define kKindDevice 5
#define kKindNetwork 6

struct PACKED NewCatalog final {
  NewCharType Name[kNewFSNodeNameLen];

  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Lba FirstFork;
  HCore::Lba LastFork;

  HCore::Lba SiblingRecords[12];
};

#define kNewFSMaxEntries 256

struct PACKED NewCatalogRecord final {
  HCore::Lba Entries[kNewFSMaxEntries];
};

struct PACKED NewFork final {
  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Int64 ResourceId;
  HCore::Int32 ResourceKind;
  HCore::Int32 ResourceFlags;

  HCore::Lba DataOffset;  // Where to look for this data?
  HCore::SizeT DataSize;  // Data size according using sector count.

  HCore::Lba NextSibling;
  HCore::Lba PreviousSibling;
};

#define kResourceTypeDialog 10
#define kResourceTypeString 11
#define kResourceTypeMenu 12

#define kConfigLen 64
#define kPartLen 32

struct PACKED NewPartitionBlock final {
  NewCharType Ident[kNewFSIdentLen];
  NewCharType PartitionName[kPartLen];

  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Lba StartCatalog;
  HCore::SizeT CatalogCount;

  HCore::SizeT DiskSize;

  HCore::SizeT FreeCatalog;
  HCore::SizeT FreeSectors;

  HCore::SizeT SectorCount;
  HCore::SizeT SectorSize;

  HCore::Char Pad[kNewFSPadLen];
};

#define kCatalogKindFile 1
#define kCatalogKindDir 2
#define kCatalogKindAlias 3

//! shared between network or
//! other filesystems. Export forks as .zip when copying.
#define kCatalogKindShared 4

#define kCatalogKindResource 5
#define kCatalogKindExecutable 6

#define kCatalogKindPage 8

#define kCatalogKindDevice 9
#define kCatalogKindLock 10

#define kFilesystemSeparator '/'

#define kFilesystemUpDir ".."
#define kFilesystemRoot "/"

#define kFilesystemLF '\r'
#define kFilesystemEOF (-1)

#define kFilesystemBitWidth (sizeof(NewCharType))
#define kFilesystemLbaType (HCore::Lba)

/// Start After the PM headers, pad 1024 bytes.
#define kNewFSAddressAsLba (1024U)

namespace HCore {
///
/// \name NewFSParser
/// \brief NewFS parser class. (catalog creation, remove removal, root, forks...)
/// Designed like the DOM, detects the filesystem automatically.
///

class NewFSParser {
 public:
  explicit NewFSParser() = default;
  virtual ~NewFSParser() = default;

 public:
  HCORE_COPY_DEFAULT(NewFSParser);

 public:
  virtual _Output NewFork* ForkFrom(NewCatalog& catalog, const Int64& id) = 0;

  virtual _Output NewCatalog* FindCatalog(const char* catalogName) = 0;

  virtual _Output NewCatalog* RootCatalog() = 0;

  virtual _Output NewCatalog* NextCatalog(_Input _Output NewCatalog& cur) = 0;

  virtual _Output NewCatalog* PrevCatalog(_Input _Output NewCatalog& cur) = 0;

  virtual _Output NewCatalog* GetCatalog(_Input const char* name) = 0;

  virtual _Output NewCatalog* CreateCatalog(_Input const char* name,
                                    _Input const Int32& flags,
                                    _Input const Int32& kind) = 0;

  virtual _Output NewCatalog* CreateCatalog(_Input const char* name) = 0;

  virtual bool WriteCatalog(_Input _Output NewCatalog& catalog,
                            voidPtr data) = 0;
  virtual bool RemoveCatalog(_Input _Output NewCatalog& catalog) = 0;

  /// @brief Make a EPM+NewFS drive out of the disk.
  /// @param drive The drive to write on.
  /// @return If it was sucessful, see DbgLastError().
  virtual bool Format(_Input _Output DriveTraits& drive) = 0;
};

///
/// \name NewFilesystemHelper
/// \brief Filesystem helper and utils.
///

class NewFilesystemHelper final {
 public:
  static const char* Root();
  static const char* UpDir();
  static const char Separator();
};
}  // namespace HCore

enum {
  kNewFSPartGPT,
  kNewFSPartEPM,
  kNewFSPartUDF,
  kNewFSPartCount,
};
