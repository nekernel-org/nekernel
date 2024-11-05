/* -------------------------------------------

	Copyright Amlal EL Mahrouss.

------------------------------------------- */

#pragma once

#define kDriveSectorSizeHDD		(512U)
#define kDriveSectorSizeSSD		(512U)
#define kDriveSectorSizeOptical (2048)

namespace Kernel
{
	template <typename T>
	class DeviceInterface;

	class NVMEDeviceInterface;
	class AHCIDeviceInterface;
	class ATADeviceInterface;
	class SCSIDeviceInterface;
} // namespace Kernel
