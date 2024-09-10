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
 * @copyright ZKA Technologies.
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

STATIC Kernel::PCI::Device kAhciDevice;
STATIC HbaPort* kAhciPort = nullptr;

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
			iterator[devIndex].Leak().EnableMmio();		 /// enable the memory i/o for this ahci device.
			iterator[devIndex].Leak().BecomeBusMaster(); /// become bus master for this ahci device, so that we can control it.

			kAhciDevice = iterator[devIndex].Leak(); /// and then leak the reference.

			HbaMem* mem_ahci = (HbaMem*)kAhciDevice.Bar();

			UInt32 ports_implemented = mem_ahci->Pi;
			Int32  ahci_index		 = 0;

			const auto cMaxAhciDevices = 32;
			const auto cAhciSig		   = 0x00000101;
			const auto cAhciPresent	   = 0x03;
			const auto cAhciIPMActive  = 0x01;

			auto detected = false;

			while (ahci_index < cMaxAhciDevices)
			{
				if (ports_implemented)
				{
					kcout << "Port is implemented by host.\r";

					UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == cAhciSig &&
						det == cAhciPresent &&
						ipm == cAhciIPMActive)
					{
						kcout << "Found AHCI controller.\r";
						kcout << "Device is of SATA type.\r";

						detected = true;

						kAhciPort = &mem_ahci->Ports[ahci_index];

						// start command engine.
						// drv_start_ahci_engine();
					}
				}

				ports_implemented >>= 1;
				ahci_index++;
			}

			return detected;
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
Kernel::SizeT drv_get_sector_count()
{
	return 0;
}

/// @brief Get the drive size.
Kernel::SizeT drv_get_size()
{
	return 0;
}
#endif // __AHCI__
