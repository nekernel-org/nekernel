/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <StorageKit/NVME.h>

namespace Kernel
{
	NVMEDeviceInterface::NVMEDeviceInterface(void (*Out)(MountpointInterface* outpacket),
											 void (*In)(MountpointInterface* inpacket),
											 void (*Cleanup)(void))
		: DeviceInterface(Out, In), fCleanup(Cleanup)
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
