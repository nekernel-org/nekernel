/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DriveManager.hxx>
#include <KernelKit/FileManager.hpp>

/** ---------------------------------------------------

    * THIS FILE CONTAINS CODE FOR THE HCFS I/O DEVICES.

------------------------------------------------------- */

#ifdef __FSKIT_NEWFS__

#include <FirmwareKit/EPM.hxx>

/// Useful macros.

#define NEWFS_WRITE(DRV, TRAITS, MP) (*MP->DRV()) << TRAITS
#define NEWFS_READ(DRV, TRAITS, MP) (*MP->DRV()) >> TRAITS

using namespace HCore;

enum {
  kHCFSSubDriveA,
  kHCFSSubDriveB,
  kHCFSSubDriveC,
  kHCFSSubDriveD,
  kHCFSSubDriveInvalid,
  kHCFSSubDriveCount,
};

Int32 ke_newfs_read(Mountpoint* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex) {
  if (!Mnt) return -1;

  switch (DrvIndex) {
    case 0: {
      NEWFS_READ(A, DrvTrait, Mnt);
      break;
    }
    case 1: {
      NEWFS_READ(B, DrvTrait, Mnt);
      break;
    }
    case 2: {
      NEWFS_READ(C, DrvTrait, Mnt);
      break;
    }
    case 3: {
      NEWFS_READ(D, DrvTrait, Mnt);
      break;
    }
  }

  return DrvTrait.fPacket.fPacketGood;
}

Int32 ke_newfs_write(Mountpoint* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex) {
  if (!Mnt) return -1;

  switch (DrvIndex) {
    case 0: {
      NEWFS_WRITE(A, DrvTrait, Mnt);
      break;
    }
    case 1: {
      NEWFS_WRITE(B, DrvTrait, Mnt);
      break;
    }
    case 2: {
      NEWFS_WRITE(C, DrvTrait, Mnt);
      break;
    }
    case 3: {
      NEWFS_WRITE(D, DrvTrait, Mnt);
      break;
    }
  }

  return DrvTrait.fPacket.fPacketGood;
}

#endif  // ifdef __FSKIT_NEWFS__
