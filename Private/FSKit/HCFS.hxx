/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <FirmwareKit/EPM.hxx>

#define kHCFSNameLen 256

/// @brief HCFS Balanced Tree structure.
/// @author Amlal El Mahrouss
typedef struct HCFSBTree final {
	HCore::WideChar fCatalogName[kHCFSNameLen];
	
    struct {
		  HCore::Lba fDataCatalog;
		  HCore::Lba fRsrcCatalog;
		  HCore::SizeT fDataCatalogSize;
		  HCore::SizeT fRsrcCatalogSize;
	} fCatalogData;
} PACKED HCFSBTree;

#define kHCFSGuid "@{0771b3c9-b977-440a-a9ca-396b6d3f07b5}"

enum {
  kHCFSHardDrive = 0xC0,          // Hard Drive
  kHCFSOpticalDrive = 0x0C,       // Blu-Ray/DVD
  kHCFSMassStorageDevice = 0xCC,  // USB
  kHCFSUnknown = 0xFF,            // unknown device or unsupported (floppy)
  kHCFSDriveCount = 4,
};

// EOF.
