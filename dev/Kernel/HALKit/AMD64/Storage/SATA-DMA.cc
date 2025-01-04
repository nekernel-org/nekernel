/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI.cc
 * @author Amlal EL Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @Copyright (C) 2024, Theater Quality Corp, all rights reserved.
 *
 */

#include "NewKit/Defines.h"
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/LPC.h>

#include <Mod/ATA/ATA.h>
#include <Mod/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#ifdef __AHCI__

#define HBA_ERR_TFE	  (1 << 30)
#define HBA_PxCMD_ST  0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR  0x4000
#define HBA_PxCMD_CR  0x8000

#define kAhciStartAddress mib_cast(4)

#define kAhciLBAMode (1 << 6)

#define kAhciMaxPoll (100000U)

#define kCmdOrCtrlCmd  1
#define kCmdOrCtrlCtrl 0

#define kAhciSRBsy 0x80
#define kAhciSRDrq 0x08

enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06,
	kSATABar5		= 0x24,
};

STATIC Kernel::PCI::Device kAhciDevice;
STATIC HbaPort* kAhciPort				   = nullptr;
STATIC Kernel::Lba kCurrentDiskSectorCount = 0UL;

template <BOOL ReadWrite, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_cnt, Kernel::SizeT size_buffer);

static Kernel::Void drv_calculate_disk_geometry() noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept
{
	kCurrentDiskSectorCount = 0UL;

	Kernel::UInt8 identify_data[kib_cast(4)] = {};

	drv_std_input_output<NO, YES, YES>(0, identify_data, 0, kib_cast(8));

	kCurrentDiskSectorCount = (identify_data[61] << 16) | identify_data[60];

	kcout << "Disk Size: " << Kernel::number(drv_get_size()) << endl;
	kcout << "Highest AHCI LBA: " << Kernel::number(kCurrentDiskSectorCount) << endl;
}

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of kAhciPort that have been detected.
/// @return if the disk was successfully initialized or not.
Kernel::Boolean drv_std_init(Kernel::UInt16& PortsImplemented)
{
	using namespace Kernel;

	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT device_index = 0; device_index < ZKA_BUS_COUNT; ++device_index)
	{
		kAhciDevice = iterator[device_index].Leak(); // And then leak the reference.

		// if SATA and then interface is AHCI...
		if (kAhciDevice.Subclass() == kSATASubClass &&
			kAhciDevice.ProgIf() == kSATAProgIfAHCI)
		{
			kAhciDevice.EnableMmio(0x24);	   // Enable the memory index_byte/o for this ahci device.
			kAhciDevice.BecomeBusMaster(0x24); // Become bus master for this ahci device, so that we can control it.

			HbaMem* mem_ahci = (HbaMem*)kAhciDevice.Bar(0x24);

			Kernel::UInt32 ports_implemented = mem_ahci->Pi;
			Kernel::UInt16 ahci_index		 = 0;

			const Kernel::UInt16 kMaxPortsImplemented = 32;
			const Kernel::UInt32 kSATASignature		  = 0x00000101;
			const Kernel::UInt8	 kAhciPresent		  = 0x03;
			const Kernel::UInt8	 kAhciIPMActive		  = 0x01;

			Kernel::Boolean detected = false;

			while (ahci_index < kMaxPortsImplemented)
			{
				if (ports_implemented)
				{
					kcout << "Port is implemented by host.\r";

					Kernel::UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					Kernel::UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature)
					{
						kcout << "Port is AHCI controller.\r";

						detected = true;

						kAhciPort = &mem_ahci->Ports[ahci_index];

						kAhciPort->Cmd &= ~HBA_PxCMD_FRE;

						// Clear FRE (bit4)
						kAhciPort->Cmd &= ~HBA_PxCMD_ST;

						// Wait until FR (bit14), CR (bit15) are cleared
						while (YES)
						{
							if (kAhciPort->Cmd & HBA_PxCMD_CR)
								continue;

							if (kAhciPort->Cmd & HBA_PxCMD_FR)
								continue;
							break;
						}

						kAhciPort->Cmd |= HBA_PxCMD_FRE;
						kAhciPort->Cmd |= HBA_PxCMD_ST;

						drv_calculate_disk_geometry();

						break;
					}
				}

				ports_implemented >>= 1;
				++ahci_index;
			}

			return detected;
		}
	}

	return No;
}

Kernel::Boolean drv_std_detected(Kernel::Void)
{
	return kAhciDevice.DeviceId() != 0xFFFF;
}

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_cnt, Kernel::SizeT size_buffer)
{
	lba /= sector_cnt;

	drv_std_input_output<YES, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_cnt, size_buffer);
}

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_cnt, Kernel::SizeT size_buffer)
{
	lba /= sector_cnt;

	drv_std_input_output<NO, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_cnt, size_buffer);
}

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_cnt, Kernel::SizeT size_buffer)
{
	volatile HbaCmdHeader* command_header = *(HbaCmdHeader**)&kAhciPort->Clb;

	command_header->Cfl = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);

	command_header->Write = Write;
	command_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1;

	HbaCmdTbl* command_table = (HbaCmdTbl*)(command_header->Ctba);

	command_table->PrdtEntries[0].Dba		   = *(Kernel::UInt32*)&buffer;
	command_table->PrdtEntries[0].Dbc		   = size_buffer;
	command_table->PrdtEntries[0].InterruptBit = 1;

	FisRegH2D* h2d_fis = (FisRegH2D*)(&command_table->Cfis);

	h2d_fis->Command   = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;
	h2d_fis->FisType   = kFISTypeRegH2D;
	h2d_fis->CmdOrCtrl = CommandOrCTRL;

	h2d_fis->Lba0 = lba & 0xFF;
	h2d_fis->Lba1 = lba >> 8;
	h2d_fis->Lba2 = lba >> 16;

	h2d_fis->Device = 1 << 6;

	h2d_fis->Lba3 = lba >> 24;
	h2d_fis->Lba4 = lba >> 32;
	h2d_fis->Lba5 = lba >> 40;

	h2d_fis->CountLow  = sector_cnt & 0xFF;
	h2d_fis->CountHigh = (sector_cnt >> 8) & 0xFF;

	kAhciPort->Ci |= (1 << 0); // Command Issue

	while (kAhciPort->Ci & (1 << 0))
	{
		if (kAhciPort->Is & HBA_ERR_TFE) // Task file error
		{
			kcout << "AHCI: Read disk error.\r";

			err_global_get() = Kernel::kErrorUnrecoverableDisk;

			return;
		}
	}

	// Check again for the last time.
	if (kAhciPort->Is & HBA_ERR_TFE) // task file error status
	{
		using namespace Kernel;

		kcout << "AHCI: Read disk error.\r";
		*buffer = 0;

		err_global_get() = kErrorUnrecoverableDisk;

		return;
	}
}

/***
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
Kernel::SizeT drv_get_sector_count()
{
	return kCurrentDiskSectorCount;
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
Kernel::SizeT drv_get_size()
{
	return (drv_get_sector_count()) * kAHCISectorSize;
}

#endif // ifdef __AHCI__
