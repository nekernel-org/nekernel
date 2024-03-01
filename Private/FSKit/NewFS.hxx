/* -------------------------------------------

    Copyright Mahrouss Logic

    File: NewFS.hxx
    Purpose:

    Revision History:

    ?/?/?: Added file (amlel)
    12/02/24: Add UUID macro for EPM and GPT partition schemes.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hpp>
#include <HintKit/CompilerHint.hxx>
#include <KernelKit/DriveManager.hpp>
#include <NewKit/Defines.hpp>

/**
    @brief HCore File System implementation.
*/

#define kNewFSInvalidFork -1
#define kNewFSInvalidCatalog -1
#define kNewFSNodeNameLen 256

#define kNewFSIdentLen 6
#define kNewFSIdent "  HCFS"
#define kPadLen 16

//! On EPM and GPT disks.
#define kNewFSUUID "DD997393-9CCE-4288-A8D5-C0FDE3908DBE"

#define kNewFSVersionInteger 0x11
#define kNewFSVerionString   "1.1"

typedef HCore::WideChar NewCharType;

enum {
  kNewFSHardDrive = 0xC0,          // Hard Drive
  kNewFSOpticalDrive = 0x0C,       // Blu-Ray/DVD
  kNewFSMassStorageDevice = 0xCC,  // USB
  kNewFSUnknown = 0xFF,            // unknown device or unsupported (floppy)
  kNewFSDriveCount = 4,
};

struct PACKED NewBootBlock final {
  NewCharType Ident[kNewFSIdentLen];
  NewCharType Shell[kNewFSNodeNameLen];

  HCore::Int64 NumParts;
  HCore::Int64 FreeSectors;
  HCore::Int64 SectorCount;
  HCore::Int64 SectorSz;

  HCore::Int64 DiskSize;
  HCore::Int32 DiskKind;

  HCore::Lba FirstPartBlock;
  HCore::Lba LastPartBlock;

  HCore::WideChar Pad[kPadLen];
};

#define kFlagDeleted 0xF0
#define kFlagUnallocated 0x0F
#define kFlagCatalog 0xFF

#define kKindCatalog 1
#define kKindDirectory 2
#define kKindSymlink 3
#define kKindPartition 4

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

  HCore::Char Pad[kPadLen];
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

#define kFilesystemCR '\r'
#define kFilesystemLF '\n'
#define kFilesystemEOF (-1)

#define kFilesystemBitWidth (sizeof(NewCharType))
#define kFilesystemLbaType (HCore::Lba)
#define kNewFSAddressAsLba (1024)

namespace HCore {
///
/// \name NewFSImplementation
/// \brief HCFS filesystem operations. (catalog creation, remove removal, root
/// fork...)
///

class NewFSImplementation {
 public:
  explicit NewFSImplementation() = default;
  virtual ~NewFSImplementation() = default;

 public:
  HCORE_COPY_DEFAULT(NewFSImplementation);

  virtual NewFork* ForkFrom(NewCatalog& catalog, const Int64& id) = 0;

  virtual NewCatalog* RootCatalog() = 0;
  virtual NewCatalog* NextCatalog(_Input _Output NewCatalog& cur) = 0;
  virtual NewCatalog* PrevCatalog(_Input _Output NewCatalog& cur) = 0;

  virtual NewCatalog* GetCatalog(_Input const char* name) = 0;

  virtual NewCatalog* CreateCatalog(_Input const char* name,
                                    _Input const Int32& flags,
                                    _Input const Int32& kind) = 0;
  virtual NewCatalog* CreateCatalog(_Input const char* name) = 0;

  virtual bool WriteCatalog(_Input _Output NewCatalog& catalog,
                            voidPtr data) = 0;
  virtual bool RemoveCatalog(_Input NewCatalog& catalog) = 0;

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
