/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/PCI/Iterator.h>

namespace NeOS::PCI
{
	Iterator::Iterator(const Types::PciDeviceKind& type)
	{
		// probe devices.
		for (int bus = 0; bus < NE_BUS_COUNT; ++bus)
		{
			for (int device = 0; device < NE_DEVICE_COUNT; ++device)
			{
				for (int function = 0; function < NE_FUNCTION_COUNT; ++function)
				{
					Device dev(bus, device, function, 0x00);

					if (dev.Class() == (UChar)type)
					{
						fDevices[bus] = dev;
					}
				}
			}
		}
	}

	Iterator::~Iterator()
	{
	}

	Ref<PCI::Device> Iterator::operator[](const Size& at)
	{
		return fDevices[at];
	}
} // namespace NeOS::PCI
