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
#include <KernelKit/DriveManager.hxx>
#include <NewKit/Defines.hpp>

/**
    @brief New File System specification.
    @author Amlal EL Mahrouss
*/

#define kNewFSInvalidFork -1
#define kNewFSInvalidCatalog -1
#define kNewFSNodeNameLen 256

#define kNewFSMinimumSectorSz 4096

#define kNewFSIdentLen 8
#define kNewFSIdent " NewFS"
#define kNewFSPadLen 16

//! On EPM and GPT disks.
#define kNewFSUUID "@{DD997393-9CCE-4288-A8D5-C0FDE3908DBE}"

#define kNewFSVersionInteger 0x121
#define kNewFSVerionString   "1.2.1"

#define kNewFSCatalogKindFile 1
#define kNewFSCatalogKindDir 2
#define kNewFSCatalogKindAlias 3

//! shared between network or
//! other filesystems. Export forks as .zip when copying.
#define kNewFSCatalogKindShared 4

#define kNewFSCatalogKindResource 5
#define kNewFSCatalogKindExecutable 6

#define kNewFSCatalogKindPage 8

#define kNewFSPartitionTypeStandard 7
#define kNewFSPartitionTypePage 8
#define kNewFSPartitionTypeBoot 9

#define kNewFSCatalogKindDevice 9
#define kNewFSCatalogKindLock 10

#define kNewFSSeparator '/'

#define kNewFSUpDir ".."
#define kNewFSRoot "/"

#define kNewFSLF '\r'
#define kNewFSEOF (-1)

#define kNewFSBitWidth (sizeof(NewCharType))
#define kNewFSLbaType (HCore::Lba)

/// Start After the PM headers, pad 1024 bytes.
#define kNewFSAddressAsLba (1024U)

#define kResourceTypeDialog 10
#define kResourceTypeString 11
#define kResourceTypeMenu 12

#define kConfigLen 64
#define kPartLen 32

#define kNewFSFlagDeleted 0xF0
#define kNewFSFlagUnallocated 0x0F
#define kNewFSFlagCatalog 0xFF

typedef HCore::Char NewCharType;

enum {
  kNewFSHardDrive = 0xC0,          // Hard Drive (SSD, HDD)
  kNewFSOpticalDrive = 0x0C,       // Blu-Ray/DVD
  kNewFSMassStorageDevice = 0xCC,  // USB
  kNewFSScsi = 0xC4,               // SCSI Hard Drive
  kNewFSUnknown = 0xFF,            // Unknown device. (floppy)
  kNewFSDriveCount = 5,
};

/// @brief Ccatalog type.
struct PACKED NewCatalog final {
  NewCharType Name[kNewFSNodeNameLen];

  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Lba FirstFork;
  HCore::Lba LastFork;

  HCore::Lba NextSibling;
  HCore::Lba PrevSibling;
};

/// @brief Fork type.
struct PACKED NewFork final {
  NewCharType  Name[kNewFSNodeNameLen];

  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Int64 ResourceId;
  HCore::Int32 ResourceKind;
  HCore::Int32 ResourceFlags;

  HCore::Lba   DataOffset;  //8 Where to look for this data?
  HCore::SizeT DataSize;  /// Data size according using sector count.

  HCore::Lba NextSibling;
  HCore::Lba PreviousSibling;
};

/// @brief Partition block type
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
  virtual _Output NewFork* GetForkFrom(NewCatalog& catalog, const Char* name) = 0;

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
  virtual bool Format(_Input _Output DriveTrait& drive) = 0;
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
