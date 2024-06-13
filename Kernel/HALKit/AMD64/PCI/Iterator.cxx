/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/PCI/Iterator.hpp>

#define PCI_ITERATOR_FIND_AND_UNWRAP(DEV, SZ) \
	if (DEV.Leak().Leak())                    \
		return *DEV.Leak().Leak();

namespace NewOS::PCI
{
	Iterator::Iterator(const Types::PciDeviceKind& type)
	{
		// probe devices.
		for (int bus = 0; bus < NEWOS_BUS_COUNT; ++bus)
		{
			for (int device = 0; device < NEWOS_DEVICE_COUNT; ++device)
			{
				for (int function = 0; function < NEWOS_FUNCTION_COUNT; ++function)
				{
					Device dev(bus, device, function, 0);

					if (dev.Class() == (UChar)type)
					{
						*fDevices[bus].Leak().Leak() = dev;
					}
				}
			}
		}
	}

	Iterator::~Iterator()
	{
	}

	Ref<PCI::Device> Iterator::operator[](const Size& sz)
	{
		PCI_ITERATOR_FIND_AND_UNWRAP(fDevices[sz], sz);
		return {};
	}
} // namespace NewOS::PCI
