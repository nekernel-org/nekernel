/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DriveManager.hpp>
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

Int32 KeHCFSRead(Mountpoint* Mnt, DriveTraits& DrvTraits, Int32 DrvIndex) {
  if (!Mnt) return -1;

  switch (DrvIndex) {
    case 0: {
      NEWFS_READ(A, DrvTraits, Mnt);
      break;
    }
    case 1: {
      NEWFS_READ(B, DrvTraits, Mnt);
      break;
    }
    case 2: {
      NEWFS_READ(C, DrvTraits, Mnt);
      break;
    }
    case 3: {
      NEWFS_READ(D, DrvTraits, Mnt);
      break;
    }
  }

  return DrvTraits.fPacket.fPacketGood;
}

Int32 KeHCFSWrite(Mountpoint* Mnt, DriveTraits& DrvTraits, Int32 DrvIndex) {
  if (!Mnt) return -1;

  switch (DrvIndex) {
    case 0: {
      NEWFS_WRITE(A, DrvTraits, Mnt);
      break;
    }
    case 1: {
      NEWFS_WRITE(B, DrvTraits, Mnt);
      break;
    }
    case 2: {
      NEWFS_WRITE(C, DrvTraits, Mnt);
      break;
    }
    case 3: {
      NEWFS_WRITE(D, DrvTraits, Mnt);
      break;
    }
  }

  return DrvTraits.fPacket.fPacketGood;
}

#endif  // ifdef __FSKIT_NEWFS__
