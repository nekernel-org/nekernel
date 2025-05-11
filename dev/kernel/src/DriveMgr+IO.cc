/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DriveMgr.h>
#include <KernelKit/FileMgr.h>

/*************************************************************
 *
 * File: DriveMgr+IO.cc
 * Purpose: Filesystem to mountpoint interface.
 * Date: 3/26/24
 *
 * Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *************************************************************/

/// Useful macros regarding the IFS.

#define fsi_ifs_write(DRV, TRAITS, MP) (MP->DRV()).fOutput(TRAITS)
#define fsi_ifs_read(DRV, TRAITS, MP) (MP->DRV()).fInput(TRAITS)

namespace Kernel {
/// @brief Read from newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Int32 fs_ifs_read(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex) {
  if (!Mnt) return 1;

  DrvTrait.fPacket.fPacketGood = false;

  switch (DrvIndex) {
    case MountpointInterface::kDriveIndexA: {
      fsi_ifs_read(A, DrvTrait.fPacket, Mnt);
      break;
    }
    case MountpointInterface::kDriveIndexB: {
      fsi_ifs_read(B, DrvTrait.fPacket, Mnt);
      break;
    }
    case MountpointInterface::kDriveIndexC: {
      fsi_ifs_read(C, DrvTrait.fPacket, Mnt);
      break;
    }
    case MountpointInterface::kDriveIndexD: {
      fsi_ifs_read(D, DrvTrait.fPacket, Mnt);
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
Int32 fs_ifs_write(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex) {
  if (!Mnt) return 1;

  DrvTrait.fPacket.fPacketGood = false;

  switch (DrvIndex) {
    case MountpointInterface::kDriveIndexA: {
      fsi_ifs_write(A, DrvTrait.fPacket, Mnt);
      break;
    }
    case MountpointInterface::kDriveIndexB: {
      fsi_ifs_write(B, DrvTrait.fPacket, Mnt);
      break;
    }
    case MountpointInterface::kDriveIndexC: {
      fsi_ifs_write(C, DrvTrait.fPacket, Mnt);
      break;
    }
    case MountpointInterface::kDriveIndexD: {
      fsi_ifs_write(D, DrvTrait.fPacket, Mnt);
      break;
    }
  }

  return DrvTrait.fPacket.fPacketGood;
}
}  // namespace Kernel