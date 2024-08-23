/* -------------------------------------------

	Copyright ZKA Technologies.

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
static HbaPort*			   kAhciPort = nullptr;

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of port that have been detected.
/// @return
Kernel::Boolean drv_std_init(Kernel::UInt16& PortsImplemented)
{
	using namespace Kernel;

	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT devIndex = 0; devIndex < ZKA_BUS_COUNT; ++devIndex)
	{
		if (iterator[devIndex].Leak().Subclass() == kSATASubClass &&
			iterator[devIndex].Leak().ProgIf() == kSATAProgIfAHCI)
		{
			iterator[devIndex].Leak().EnableMmio();	 /// enable the memory i/o for this ahci device.
			kAhciDevice = iterator[devIndex].Leak(); /// and then leak the reference.

			HbaMem* mem_ahci = (HbaMem*)kAhciDevice.Bar();

			UInt32 pi = mem_ahci->Pi;
			Int32  i  = 0;

			const auto cMaxAhciDevices = 32;
			const auto cAhciSig		   = 0x00000101;
			const auto cAhciPresent	   = 0x03;

			while (i < cMaxAhciDevices)
			{
				if (pi & 1 &&
					(mem_ahci->Ports[i].Ssts & 0x0F) == cAhciPresent &&
					((mem_ahci->Ports[i].Ssts >> 8) & 0x0F) == 1)
				{
					kcout << "newoskrnl: Port is implemented.\r";

					if (mem_ahci->Ports[i].Sig == cAhciSig)
					{
						kcout << "newoskrnl: device is SATA.\r";
					}
				}

				pi >>= 1;
				i++;
			}

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
/***
	@brief Getter, gets the number of sectors inside the drive.
*/
Kernel::SizeT drv_std_get_sector_count()
{
	return 0;
}

/// @brief Get the drive size.
Kernel::SizeT drv_std_get_drv_size()
{
	return 0;
}
#endif // __AHCI__
