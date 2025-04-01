/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <KernelKit/PCI/Device.h>

#define PCI_BAR_IO		 0x01
#define PCI_BAR_LOWMEM	 0x02
#define PCI_BAR_64		 0x04
#define PCI_BAR_PREFETCH 0x08

Kernel::UInt NE_PCIReadRaw(Kernel::UInt bar, Kernel::UShort bus, Kernel::UShort dev, Kernel::UShort fun)
{
	Kernel::UInt target = 0x80000000 | ((Kernel::UInt)bus << 16) |
						  ((Kernel::UInt)dev << 11) | ((Kernel::UInt)fun << 8) |
						  (bar & 0xFC);

	Kernel::HAL::rt_out32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigAddress,
						  target);

	Kernel::HAL::rt_wait_400ns();

	return Kernel::HAL::rt_in32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigData);
}

void NE_PCISetCfgTarget(Kernel::UInt bar, Kernel::UShort bus, Kernel::UShort dev, Kernel::UShort fun)
{
	Kernel::UInt target = 0x80000000 | ((Kernel::UInt)bus << 16) |
						  ((Kernel::UInt)dev << 11) | ((Kernel::UInt)fun << 8) |
						  (bar & 0xFC);

	Kernel::HAL::rt_out32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigAddress,
						  target);

	Kernel::HAL::rt_wait_400ns();
}

namespace Kernel::PCI
{
	Device::Device(UShort bus, UShort device, UShort func, UInt32 bar)
		: fBus(bus), fDevice(device), fFunction(func), fBar(bar)
	{
	}

	Device::~Device() = default;

	UInt Device::Read(UInt bar, Size sz)
	{
		// Ensure aligned access by masking to 4-byte boundary
		NE_PCISetCfgTarget(bar & 0xFC, fBus, fDevice, fFunction);

		// Read 4 bytes and shift out the correct value
		UInt data = HAL::rt_in32((UShort)PciConfigKind::ConfigData);

		if (sz == 4)
			return data;
		if (sz == 2)
			return (data >> ((bar & 2) * 8)) & 0xFFFF;
		if (sz == 1)
			return (data >> ((bar & 3) * 8)) & 0xFF;

		return (UShort)PciConfigKind::Invalid;
	}

	void Device::Write(UInt bar, UIntPtr data, Size sz)
	{
		NE_PCISetCfgTarget(bar & 0xFC, fBus, fDevice, fFunction);

		if (sz == 4)
			HAL::rt_out32((UShort)PciConfigKind::ConfigData, (UInt)data);
		else if (sz == 2)
		{
			UInt temp = HAL::rt_in32((UShort)PciConfigKind::ConfigData);
			temp &= ~(0xFFFF << ((bar & 2) * 8));
			temp |= (data & 0xFFFF) << ((bar & 2) * 8);
			HAL::rt_out32((UShort)PciConfigKind::ConfigData, temp);
		}
		else if (sz == 1)
		{
			UInt temp = HAL::rt_in32((UShort)PciConfigKind::ConfigData);
			temp &= ~(0xFF << ((bar & 3) * 8));
			temp |= (data & 0xFF) << ((bar & 3) * 8);
			HAL::rt_out32((UShort)PciConfigKind::ConfigData, temp);
		}
	}

	UShort Device::DeviceId()
	{
		return (UShort)(NE_PCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
	}

	UShort Device::VendorId()
	{
		return (UShort)(NE_PCIReadRaw(0x0, fBus, fDevice, fFunction) & 0xFFFF);
	}

	UShort Device::InterfaceId()
	{
		return (UShort)(NE_PCIReadRaw(0x09, fBus, fDevice, fFunction) >> 16);
	}

	UChar Device::Class()
	{
		return (UChar)(NE_PCIReadRaw(0x08, fBus, fDevice, fFunction) >> 24);
	}

	UChar Device::Subclass()
	{
		return (UChar)(NE_PCIReadRaw(0x08, fBus, fDevice, fFunction) >> 16);
	}

	UChar Device::ProgIf()
	{
		return (UChar)(NE_PCIReadRaw(0x08, fBus, fDevice, fFunction) >> 8);
	}

	UChar Device::HeaderType()
	{
		return (UChar)(NE_PCIReadRaw(0xC, fBus, fDevice, fFunction) >> 16);
	}

	void Device::EnableMmio()
	{
		UInt32 command = Read(0x04, sizeof(UInt32));
		command |= (1 << 1); // Memory Space Enable (bit 1)

		Write(0x04, command, sizeof(UInt32));
	}

	void Device::BecomeBusMaster()
	{
		UInt32 command = Read(0x04, sizeof(UInt32));
		command |= (1 << 2); // Bus Master Enable (bit 2)
		Write(0x04, command, sizeof(UInt32));
	}

	UIntPtr Device::Bar(UInt32 bar_in)
	{
		UInt32 bar = NE_PCIReadRaw(bar_in, fBus, fDevice, fFunction);

		if (bar & PCI_BAR_IO)
			return static_cast<UIntPtr>(bar & ~0x03);

		if (bar & PCI_BAR_64)
		{
			UInt32 high = NE_PCIReadRaw((bar_in + 4) & ~0x03, fBus, fDevice, fFunction);
			return (static_cast<UIntPtr>(high) << 32) | (bar & ~0x0F);
		}

		return static_cast<UIntPtr>(bar & ~0x0F);
	}

	UShort Device::Vendor()
	{
		UShort vendor = this->VendorId();
		return vendor;
	}

	Device::operator bool()
	{
		return this->VendorId() != (UShort)PciConfigKind::Invalid;
	}
} // namespace Kernel::PCI
