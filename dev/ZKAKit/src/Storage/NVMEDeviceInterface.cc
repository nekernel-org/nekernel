/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/NVME.h>

namespace Kernel
{
	NVMEDeviceInterface::NVMEDeviceInterface(void (*out)(MountpointInterface* outpacket),
											 void (*in)(MountpointInterface* inpacket),
											 void (*cleanup)(void))
		: IDeviceObject(out, in), fCleanup(cleanup)
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
