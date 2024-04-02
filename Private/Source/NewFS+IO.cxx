/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DriveManager.hxx>
#include <KernelKit/FileManager.hpp>

/*************************************************************
 * 
 * File: NewFS+IO.cxx 
 * Purpose: Filesystem to mountpoint interface.
 * Date: 3/26/24
 * 
 * Copyright Mahrouss Logic, all rights reserved.
 * 
 *************************************************************/


#ifdef __FSKIT_NEWFS__

#include <FirmwareKit/EPM.hxx>

/// Useful macros.

#define NEWFS_WRITE(DRV, TRAITS, MP) (*MP->DRV()) << TRAITS
#define NEWFS_READ(DRV, TRAITS, MP) (*MP->DRV()) >> TRAITS

using namespace NewOS;

/// @brief Read from newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return 
Int32 fs_newfs_read_raw(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex) {
  if (!Mnt) return -1;

  DrvTrait.fPacket.fPacketGood = false;
  
  switch (DrvIndex) {
    case kHCFSSubDriveA: {
      NEWFS_READ(A, DrvTrait, Mnt);
      break;
    }
    case kHCFSSubDriveB: {
      NEWFS_READ(B, DrvTrait, Mnt);
      break;
    }
    case kHCFSSubDriveC: {
      NEWFS_READ(C, DrvTrait, Mnt);
      break;
    }
    case kHCFSSubDriveD: {
      NEWFS_READ(D, DrvTrait, Mnt);
      break;
    }
  }

  return DrvTrait.fPacket.fPacketGood;
}

/// @brief Write to newfs disk.
/// @param Mnt mounted interface. 
/// @param DrvTrait drive info 
/// @param DrvIndex drive index.
/// @return 
Int32 fs_newfs_write_raw(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex) {
  if (!Mnt) return -1;

  DrvTrait.fPacket.fPacketGood = false;

  switch (DrvIndex) {
    case kHCFSSubDriveA: {
      NEWFS_WRITE(A, DrvTrait, Mnt);
      break;
    }
    case kHCFSSubDriveB: {
      NEWFS_WRITE(B, DrvTrait, Mnt);
      break;
    }
    case kHCFSSubDriveC: {
      NEWFS_WRITE(C, DrvTrait, Mnt);
      break;
    }
    case kHCFSSubDriveD: {
      NEWFS_WRITE(D, DrvTrait, Mnt);
      break;
    }
  }

  return DrvTrait.fPacket.fPacketGood;
}

#endif  // ifdef __FSKIT_NEWFS__
