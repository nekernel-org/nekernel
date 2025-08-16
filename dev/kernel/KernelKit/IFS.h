/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DriveMgr.h>

namespace Kernel {
/// @brief Read from IFS disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Int32 fs_ifs_read(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex);

/// @brief Write to IFS disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Int32 fs_ifs_write(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex);
}  // namespace Kernel
