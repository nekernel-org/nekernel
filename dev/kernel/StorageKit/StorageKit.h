/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#define kDriveSectorSizeHDD		(512U)
#define kDriveSectorSizeSSD		(512U)
#define kDriveSectorSizeOptical (2048)

namespace NeOS
{
	template <typename T>
	class IDeviceObject;

	class NVMEDeviceInterface;
	class AHCIDeviceInterface;
	class ATADeviceInterface;
	class SCSIDeviceInterface;
} // namespace NeOS
