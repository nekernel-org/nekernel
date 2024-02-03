/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CompilerKit/CompilerKit.hpp>
#include <KernelKit/DriveManager.hpp>
#include <NewKit/Defines.hpp>

#define kInvalidFork -1
#define kInvalidCatalog -1
#define kNameLen 256

#define kNewFSIdentLen 4
#define kNewFSIdent "HCFS"
#define kPadLen 16

#define kNewFSVersion 1

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
  NewCharType Shell[kNameLen];

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
  NewCharType Name[kNameLen];

  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Lba FirstFork;
  HCore::Lba LastFork;
};

struct PACKED NewFork final {
  HCore::Int32 Flags;
  HCore::Int32 Kind;

  HCore::Int64 ID;

  HCore::Int64 ResourceId;
  HCore::Int32 ResourceKind;
  HCore::Int32 ResourceFlags;

  HCore::Lba DataOffset;
  HCore::SizeT DataSize;

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

#define kFilesystemSeparator '\\'

#define kFilesystemUpDir ".."
#define kFilesystemRoot "\\"

#define kFilesystemLE '\r'
#define kFilesystemEOF (-1)

#define kFilesystemBitWidth sizeof(NewCharType)
#define kFilesystemLbaType HCore::Lba

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

  virtual NewCatalog* RootCatalog(void) = 0;
  virtual NewCatalog* NextCatalog(NewCatalog& cur) = 0;
  virtual NewCatalog* PrevCatalog(NewCatalog& cur) = 0;

  virtual NewCatalog* GetCatalog(const char* name) = 0;

  virtual NewCatalog* CreateCatalog(const char* name, const Int32& flags,
                                    const Int32& kind) = 0;
  virtual NewCatalog* CreateCatalog(const char* name) = 0;

  virtual bool WriteCatalog(NewCatalog& catalog, voidPtr data) = 0;
  virtual bool RemoveCatalog(NewCatalog& catalog) = 0;

  virtual bool Format(DriveTraits& drive) = 0;
};

///
/// \name MeFilesystemHelper
/// Filesystem helper and utils.
///

class MeFilesystemHelper final {
 public:
  static const char* Root() { return kFilesystemRoot; }
  static const char* UpDir() { return kFilesystemUpDir; }
  static const char Separator() { return kFilesystemSeparator; }
};
}  // namespace HCore

#define kNewFSAddressAsLba 1024

enum {
  kNewFSPartGPT,
  kNewFSPartEPM,
  kNewFSPartUDF,
  kNewFSPartCount,
};
