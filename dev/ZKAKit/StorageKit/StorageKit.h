/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#define kDriveSectorSizeHDD		(512U)
#define kDriveSectorSizeSSD		(512U)
#define kDriveSectorSizeOptical (2048)

namespace Kernel
{
	template <typename T>
	class IDeviceObject;

	class NVMEDeviceInterface;
	class AHCIDeviceInterface;
	class ATADeviceInterface;
	class SCSIDeviceInterface;
} // namespace Kernel
