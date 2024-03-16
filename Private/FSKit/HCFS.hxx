/* -------------------------------------------

    Copyright Mahrouss Logic

    3/16/24: 

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <FirmwareKit/EPM.hxx>

#define kHCFSIdentLen 8
#define kHCFSNameLen 256
#define kHCFSPadLen 16

#define kHCFSGuid "@{0771b3c9-b977-440a-a9ca-396b6d3f07b5}"

/// @brief HCFS Balanced Tree structure.
/// @author Amlal El Mahrouss
typedef struct HCFSBTree final {
  /// @brief The Catalog Data record.
  struct PACKED {
		  HCore::Lba fDataCatalog;
		  HCore::Lba fRsrcCatalog;
		  HCore::SizeT fDataCatalogSize;
		  HCore::SizeT fRsrcCatalogSize;
	} fCatalogData;

  HCore::Lba fRelatedTrees[12];
  HCore::Char fReserved[384];
} PACKED HCFSBTree;

typedef struct HCFSCatalog {
	HCore::Char fCatalogName[kHCFSNameLen];
	HCore::UInt32 fCatalogKind;
  HCore::UInt32 fCatalogFlags;

  /// @brief Tree information structure.
  /// 0: BTree LBA.
  /// 1: Next BTree LBA.
  /// 3: Last BTree LBA.
  /// 4: First BTree LBA.
  /// Everything else is reserved.
  HCore::Lba fTreeInfo[12];

  HCore::Char fReserved[152];
} HCFSCatalog;

enum {
  kHCFSHardDrive = 0xC0,          // Hard Drive
  kHCFSOpticalDrive = 0x0C,       // Blu-Ray/DVD
  kHCFSMassStorageDevice = 0xCC,  // USB
  kHCFSUnknown = 0xFF,            // unknown device or unsupported (floppy)
  kHCFSDriveCount = 4,
};

/// @brief This is a boot block, used by HCFS to boot the system (if formated using this filesystem.)
typedef struct HCFSBootBlock final {
  HCore::Char Ident[kHCFSIdentLen];
  HCore::Char Shell[kHCFSNameLen];

  HCore::Int64 NumParts; // number of sub-partitions.
  HCore::Int64 FreeSectors;
  HCore::Int64 SectorCount;
  HCore::Int64 SectorSz;

  HCore::Int64 DiskSize; // size of media.
  HCore::Int32 DiskKind; // kind of disk.

  HCore::Lba FirstPartBlock;
  HCore::Lba LastPartBlock;

  HCore::Char Pad[kHCFSPadLen];
} PACKED HCFSBootBlock;

// EOF.
