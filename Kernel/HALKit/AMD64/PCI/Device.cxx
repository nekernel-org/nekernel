/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/PCI/Device.hxx>

Kernel::UInt NewOSPCIReadRaw(Kernel::UInt bar, Kernel::UShort bus, Kernel::UShort dev, Kernel::UShort fun)
{
	Kernel::UInt target = 0x80000000 | ((Kernel::UInt)bus << 16) |
						  ((Kernel::UInt)dev << 11) | ((Kernel::UInt)fun << 8) |
						  (bar & 0xFC);

	Kernel::HAL::Out32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigAddress,
					   target);

	return Kernel::HAL::In32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigData);
}

void NewOSPCISetCfgTarget(Kernel::UInt bar, Kernel::UShort bus, Kernel::UShort dev, Kernel::UShort fun)
{
	Kernel::UInt target = 0x80000000 | ((Kernel::UInt)bus << 16) |
						  ((Kernel::UInt)dev << 11) | ((Kernel::UInt)fun << 8) |
						  (bar & ~3);

	Kernel::HAL::Out32((Kernel::UShort)Kernel::PCI::PciConfigKind::ConfigAddress,
					   target);
}

namespace Kernel::PCI
{
	Device::Device(UShort bus, UShort device, UShort func, UShort bar)
		: fBus(bus), fDevice(device), fFunction(func), fBar(bar)
	{
	}

	Device::~Device()
	{
	}

	UInt Device::Read(UInt bar, Size sz)
	{
		NewOSPCISetCfgTarget(bar, fBus, fDevice, fFunction);

		if (sz == 4)
			return HAL::In32((UShort)PciConfigKind::ConfigData + (fBar & 3));
		if (sz == 2)
			return HAL::In16((UShort)PciConfigKind::ConfigData + (fBar & 3));
		if (sz == 1)
			return HAL::In8((UShort)PciConfigKind::ConfigData + (fBar & 3));

		return 0xFFFF;
	}

	void Device::Write(UInt bar, UIntPtr data, Size sz)
	{
		NewOSPCISetCfgTarget(bar, fBus, fDevice, fFunction);

		if (sz == 4)
			HAL::Out32((UShort)PciConfigKind::ConfigData + (fBar & 3), (UInt)data);
		if (sz == 2)
			HAL::Out16((UShort)PciConfigKind::ConfigData + (fBar & 3), (UShort)data);
		if (sz == 1)
			HAL::Out8((UShort)PciConfigKind::ConfigData + (fBar & 3), (UChar)data);
	}

	UShort Device::DeviceId()
	{
		return (UShort)(NewOSPCIReadRaw(0x0 >> 16, fBus, fDevice, fFunction));
	}

	UShort Device::VendorId()
	{
		return (UShort)(NewOSPCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
	}

	UShort Device::InterfaceId()
	{
		return (UShort)(NewOSPCIReadRaw(0x0, fBus, fDevice, fFunction) >> 16);
	}

	UChar Device::Class()
	{
		return (UChar)(NewOSPCIReadRaw(0x08, fBus, fDevice, fFunction) >> 24);
	}

	UChar Device::Subclass()
	{
		return (UChar)(NewOSPCIReadRaw(0x08, fBus, fDevice, fFunction) >> 16);
	}

	UChar Device::ProgIf()
	{
		return (UChar)(NewOSPCIReadRaw(0x08, fBus, fDevice, fFunction) >> 8);
	}

	UChar Device::HeaderType()
	{
		return (UChar)(NewOSPCIReadRaw(0xC, fBus, fDevice, fFunction) >> 16);
	}

	void Device::EnableMmio()
	{
		bool enable = Read(0x04, sizeof(UChar)) | (1 << 1);
		Write(0x04, enable, sizeof(UShort));
	}

	void Device::BecomeBusMaster()
	{
		bool enable = Read(0x04, sizeof(UShort)) | (1 << 2);
		Write(0x04, enable, sizeof(UShort));
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
