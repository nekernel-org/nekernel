/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

#include <StorageKit/NVME.h>

namespace Kernel
{
	NVMEDeviceInterface::NVMEDeviceInterface(void (*out)(MountpointInterface* outpacket),
											 void (*in)(MountpointInterface* inpacket),
											 void (*cleanup)(void))
		: DeviceInterface(out, in), fCleanup(cleanup)
	{
	}

	NVMEDeviceInterface::~NVMEDeviceInterface()
	{
		if (fCleanup)
			fCleanup();
	}

	const Char* NVMEDeviceInterface::Name() const
	{
		return ("NVMEDeviceInterface");
	}
} // namespace Kernel
