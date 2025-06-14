/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#define kDriveSectorSizeHDD (512U)
#define kDriveSectorSizeSSD (512U)
#define kDriveSectorSizeOptical (2048U)

namespace Kernel {
template <typename T>
class DeviceInterface;

class NVMEDeviceInterface;
class AHCIDeviceInterface;
class ATADeviceInterface;
class SCSIDeviceInterface;
}  // namespace Kernel
