/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/PCI/Device.h>

Kernel::UInt ZKA_PCIReadRaw(Kernel::UInt bar, Kernel::UShort bus, Kernel::UShort dev, Kernel::UShort fun)
{
	Kernel::UInt target = 0x80000000 | ((Kernel::UInt)bus << 16) |
						  ((Kernel::UInt)dev << 11) | ((Kernel::UInt)fun << 8) |
						  (bar & 0xFC);

	Kernel::HAL::rt_out32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigAddress,
						  target);

	return Kernel::HAL::rt_in32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigData);
}

void ZKA_PCISetCfgTarget(Kernel::UInt bar, Kernel::UShort bus, Kernel::UShort dev, Kernel::UShort fun)
{
	Kernel::UInt target = 0x80000000 | ((Kernel::UInt)bus << 16) |
						  ((Kernel::UInt)dev << 11) | ((Kernel::UInt)fun << 8) |
						  (bar & ~3);

	Kernel::HAL::rt_out32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigAddress,
						  target);
}

#define PCI_BAR_IO		 0x01
#define PCI_BAR_LOWMEM	 0x02
#define PCI_BAR_64		 0x04
#define PCI_BAR_PREFETCH 0x08

namespace Kernel::PCI
{
	Device::Device(UShort bus, UShort device, UShort func, UInt32 bar)
		: fBus(bus), fDevice(device), fFunction(func), fBar(bar)
	{
	}

	Device::~Device() = default;

	UInt Device::Read(UInt bar, Size sz)
	{
		ZKA_PCISetCfgTarget(bar, fBus, fDevice, fFunction);

		if (sz == 4)
			return HAL::rt_in32((UShort)PciConfigKind::ConfigData + (bar & 3));
		if (sz == 2)
			return HAL::rt_in16((UShort)PciConfigKind::ConfigData + (bar & 3));
		if (sz == 1)
			return HAL::rt_in8((UShort)PciConfigKind::ConfigData + (bar & 3));

		return 0xFFFF;
	}

	void Device::Write(UInt bar, UIntPtr data, Size sz)
	{
		ZKA_PCISetCfgTarget(bar, fBus, fDevice, fFunction);

		if (sz == 4)
			HAL::rt_out32((UShort)PciConfigKind::ConfigData + (fBar & 3), (UInt)data);
		if (sz == 2)
			HAL::rt_out16((UShort)PciConfigKind::ConfigData + (fBar & 3), (UShort)data);
		if (sz == 1)
			HAL::rt_out8((UShort)PciConfigKind::ConfigData + (fBar & 3), (UChar)data);
	}

	UShort Device::DeviceId()
	{
		return (UShort)(ZKA_PCIReadRaw(0x0 >> 16, fBus, fDevice, fFunction));
	}

	UShort Device::VendorId()
	{
		return (UShort)(ZKA_PCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
	}

	UShort Device::InterfaceId()
	{
		return (UShort)(ZKA_PCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
	}

	UChar Device::Class()
	{
		return (UChar)(ZKA_PCIReadRaw(0x08, fBus, fDevice, fFunction) >> 24);
	}

	UChar Device::Subclass()
	{
		return (UChar)(ZKA_PCIReadRaw(0x08, fBus, fDevice, fFunction) >> 16);
	}

	UChar Device::ProgIf()
	{
		return (UChar)(ZKA_PCIReadRaw(0x08, fBus, fDevice, fFunction) >> 8);
	}

	UChar Device::HeaderType()
	{
		return (UChar)(ZKA_PCIReadRaw(0xC, fBus, fDevice, fFunction) >> 16);
	}

	void Device::EnableMmio(UInt32 bar_in)
	{
		bool enable = Read(bar_in, sizeof(UShort)) | (1 << 1);
		Write(bar_in, enable, sizeof(UShort));
	}

	void Device::BecomeBusMaster(UInt32 bar_in)
	{
		bool enable = Read(bar_in, sizeof(UShort)) | (1 << 2);
		Write(bar_in, enable, sizeof(UShort));
	}

	UIntPtr Device::Bar(UInt32 bar_in)
	{
		UInt32 bar = ZKA_PCIReadRaw(bar_in, fBus, fDevice, fFunction);
		return bar;
	}

	UShort Device::Vendor()
	{
		UShort vendor = VendorId();

		if (vendor != (UShort)PciConfigKind::Invalid)
			fDevice = (UShort)Read(0x0, sizeof(UShort));

		return fDevice;
	}

	Device::operator bool()
	{
		return VendorId() != (UShort)PciConfigKind::Invalid;
	}
} // namespace Kernel::PCI
