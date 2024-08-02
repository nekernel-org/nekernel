/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

/**
 * @file AHCI.cxx
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) ZKA Technologies
 *
 */

#include <Modules/AHCI/AHCI.hxx>
#include <KernelKit/PCI/Iterator.hxx>

#ifdef __AHCI__
enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06
};

static Kernel::PCI::Device kAhciDevice;

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of port that have been detected.
/// @return
Kernel::Boolean drv_std_init(Kernel::UInt16& PortsImplemented)
{
	using namespace Kernel;

	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);
	for (SizeT devIndex = 0; devIndex < NEWOS_BUS_COUNT; ++devIndex)
	{
		if (iterator[devIndex].Leak().Subclass() == kSATASubClass &&
			iterator[devIndex].Leak().ProgIf() == kSATAProgIfAHCI)
		{
			iterator[devIndex].Leak().EnableMmio();	 /// enable the memory i/o for this ahci device.
			kAhciDevice = iterator[devIndex].Leak(); /// and then leak the reference.

			kcout << "newoskrnl: [PCI] Found AHCI controller.\r";

			return true;
		}
	}

	return false;
}

Kernel::Boolean drv_std_detected(Kernel::Void)
{
	return kAhciDevice.DeviceId() != 0xFFFF;
}

Kernel::Void drv_std_read(Kernel::UInt64 Lba, Kernel::Char* Buf, Kernel::SizeT SectorSz, Kernel::SizeT Size)
{
}

Kernel::Void drv_std_write(Kernel::UInt64 Lba, Kernel::Char* Buf, Kernel::SizeT SectorSz, Kernel::SizeT Size)
{
}
#endif // __AHCI__
