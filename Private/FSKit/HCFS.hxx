/* -------------------------------------------

    Copyright Mahrouss Logic

    3/16/24: 

------------------------------------------- */

#pragma once

/**
    @brief NewOS File System.
    @author Amlal EL Mahrouss
*/

#include <NewKit/Defines.hpp>
#include <FirmwareKit/EPM.hxx>

#define kHCFSIdentLen 8
#define kHCFSIdent "  HCFS"
#define kHCFSNameLen 256
#define kHCFSPadLen 16

#define kHCFSFlagDeleted 0xF0
#define kHCFSFlagUnallocated 0x0F
#define kHCFSFlagCatalog 0xFF

#define kHCFSGuid "@{0771b3c9-b977-440a-a9ca-396b6d3f07b5}"

/// @brief HCFS Balanced Tree structure.
/// @author Amlal El Mahrouss
typedef struct HCFSBTree final {
  /// @brief The Catalog Data record.
  struct PACKED {
		  NewOS::Lba fDataCatalog;
		  NewOS::Lba fRsrcCatalog;
		  NewOS::SizeT fDataCatalogSize;
		  NewOS::SizeT fRsrcCatalogSize;
	} fCatalogData;

  NewOS::Lba fRelatedTrees[12];
  NewOS::Char fReserved[384];
} PACKED HCFSBTree;

/// @brief Catalog file for HCFS.
typedef struct HCFSCatalog {
	NewOS::Char fCatalogName[kHCFSNameLen];
	NewOS::UInt32 fCatalogKind;
  NewOS::UInt32 fCatalogFlags;

  /// @brief Tree information structure.
  /// 0: BTree LBA.
  /// 1: Next BTree LBA.
  /// 3: Last BTree LBA.
  /// 4: First BTree LBA.
  /// Everything else is reserved.
  NewOS::Lba fTreeInfo[12];

  NewOS::Char fReserved[152];
} HCFSCatalog;

#define kHCFSCatalogKindFile 1
#define kHCFSCatalogKindDirectory 2
#define kHCFSCatalogKindJunction 3
#define kHCFSCatalogKindNetwork 4

enum {
  kHCFSHardDrive = 0xC0,          // Hard Drive
  kHCFSOpticalDrive = 0x0C,       // Blu-Ray/DVD
  kHCFSMassStorageDevice = 0xCC,  // USB
  kHCFSUnknown = 0xFF,            // unknown device or unsupported (floppy)
  kHCFSDriveCount = 4,
};

/// @brief This is a boot block, used by HCFS to boot the system (if formated using this filesystem.)
typedef struct HCFSBootBlock final {
  NewOS::Char Ident[kHCFSIdentLen];
  NewOS::Char Shell[kHCFSNameLen];

  NewOS::Int64 NumParts; // number of sub-partitions.
  NewOS::Int64 FreeSectors;
  NewOS::Int64 SectorCount;
  NewOS::Int64 SectorSz;

  NewOS::Int64 DiskSize; // size of media.
  NewOS::Int32 DiskKind; // kind of disk.

  NewOS::Lba FirstPartBlock;
  NewOS::Lba LastPartBlock;

  NewOS::Char Pad[kHCFSPadLen];
} PACKED HCFSBootBlock;

// EOF.
