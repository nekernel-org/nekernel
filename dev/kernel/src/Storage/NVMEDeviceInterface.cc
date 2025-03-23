/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <StorageKit/NVME.h>

namespace NeOS
{
	NVMEDeviceInterface::NVMEDeviceInterface(void (*out)(IDeviceObject*, MountpointInterface* outpacket),
											 void (*in)(IDeviceObject*, MountpointInterface* inpacket),
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
		return ("/dev/nvme{}");
	}
} // namespace NeOS
