/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <KernelKit/PCI/Iterator.hxx>

namespace Kernel::PCI
{
	Iterator::Iterator(const Types::PciDeviceKind& type)
	{
		// probe devices.
		for (int bus = 0; bus < ZKA_BUS_COUNT; ++bus)
		{
			for (int device = 0; device < ZKA_DEVICE_COUNT; ++device)
			{
				for (int function = 0; function < ZKA_FUNCTION_COUNT; ++function)
				{
					Device dev(bus, device, function, type == Types::PciDeviceKind::MassStorageController ? 5 : 1);

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
} // namespace Kernel::PCI
