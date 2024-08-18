/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#define kDriveSectorSizeHDD		(512)
#define kDriveSectorSizeSSD		(4096)
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
