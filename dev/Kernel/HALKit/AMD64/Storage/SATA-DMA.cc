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

#define kSataLBAMode (1 << 6)

#define kAhciSRBsy 0x80
#define kAhciSRDrq 0x08

#define kAhciPortCnt 32

enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06,
	kSATABar5		= 0x24,
};

STATIC Kernel::PCI::Device kPCIDevice;
STATIC HbaPort* kSATAPort				   = nullptr;
STATIC Kernel::Lba kCurrentDiskSectorCount = 0UL;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept;

static Kernel::Int32 drv_find_cmd_slot(HbaPort* port) noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept
{
	kCurrentDiskSectorCount = 0UL;

	Kernel::UInt8 identify_data[kib_cast(4)] = {};

	drv_std_input_output<NO, YES, YES>(0, identify_data, 0, kib_cast(8));

	kCurrentDiskSectorCount = (identify_data[61] << 16) | identify_data[60];

	kcout << "Disk Size: " << Kernel::number(drv_get_size()) << endl;
	kcout << "Highest LBA: " << Kernel::number(kCurrentDiskSectorCount) << endl;
}

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of kSATAPort that have been detected.
/// @return if the disk was successfully initialized or not.
Kernel::Boolean drv_std_init(Kernel::UInt16& PortsImplemented)
{
	using namespace Kernel;

	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT device_index = 0; device_index < ZKA_BUS_COUNT; ++device_index)
	{
		kPCIDevice = iterator[device_index].Leak(); // And then leak the reference.

		// if SATA and then interface is AHCI...
		if (kPCIDevice.Subclass() == kSATASubClass &&
			kPCIDevice.ProgIf() == kSATAProgIfAHCI)
		{
			kPCIDevice.EnableMmio(0x24);	  // Enable the memory index_byte/o for this ahci device.
			kPCIDevice.BecomeBusMaster(0x24); // Become bus master for this ahci device, so that we can control it.

			HbaMem* mem_ahci = (HbaMem*)kPCIDevice.Bar(0x24);

			Kernel::UInt32 ports_implemented = mem_ahci->Pi;
			Kernel::UInt16 ahci_index		 = 0;

			const Kernel::UInt16 kMaxPortsImplemented = kAhciPortCnt;
			const Kernel::UInt32 kSATASignature		  = 0x00000101;
			const Kernel::UInt8	 kAhciPresent		  = 0x03;
			const Kernel::UInt8	 kAhciIPMActive		  = 0x01;

			Kernel::Boolean detected = false;

			while (ahci_index < kMaxPortsImplemented)
			{
				if (ports_implemented)
				{
					kcout << "Port is implemented.\r";

					Kernel::UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					Kernel::UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature)
					{
						kcout << "Port is SATA.\r";

						kSATAPort = &mem_ahci->Ports[ahci_index];

						while (kSATAPort->Cmd & HBA_PxCMD_CR)
							;

						kSATAPort->Cmd |= HBA_PxCMD_FRE;
						kSATAPort->Cmd |= HBA_PxCMD_ST;

						drv_calculate_disk_geometry();

						detected = YES;

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
	return kPCIDevice.DeviceId() != 0xFFFF;
}

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	lba /= sector_sz;

	drv_std_input_output<YES, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer);
}

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	lba /= sector_sz;

	drv_std_input_output<NO, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer);
}

static Kernel::Int32 drv_find_cmd_slot(HbaPort* port) noexcept
{
	Kernel::UInt32 slots = (port->Sact | port->Ci);

	for (Kernel::Int32 i = 0; i < kAhciPortCnt; i++)
	{
		if ((slots & 1) == 0)
			return i;

		slots >>= 1;
	}

	return -1;
}

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept
{
	if (!CommandOrCTRL)
		return;

	volatile HbaCmdHeader* command_header = (HbaCmdHeader*)kSATAPort->Clb;

	auto slot = drv_find_cmd_slot(kSATAPort);

	if (slot == -1)
		return;

	command_header += slot;

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	command_header->Write = Write;
	command_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1;

	volatile HbaCmdTbl* command_table = (HbaCmdTbl*)(command_header->Ctba);

	command_table->PrdtEntries[0].Dba		   = (*(Kernel::UInt32*)buffer & __UINT32_MAX__);
	command_table->PrdtEntries[0].Dbau		   = ((*(Kernel::UInt32*)buffer >> 8) & __UINT32_MAX__);
	command_table->PrdtEntries[0].Dbc		   = size_buffer - 1;
	command_table->PrdtEntries[0].InterruptBit = 1;

	volatile FisRegH2D* h2d_fis = (FisRegH2D*)(&command_table->Cfis);

	if (CommandOrCTRL)
	{
		h2d_fis->Command = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

		if (Identify)
			h2d_fis->Command = kAHCICmdIdentify;

		h2d_fis->CmdOrCtrl = 1;
	}

	h2d_fis->FisType = kFISTypeRegH2D;

	h2d_fis->Device = kSataLBAMode;

	h2d_fis->Lba0 = lba & 0xFF;
	h2d_fis->Lba1 = lba >> 8;
	h2d_fis->Lba2 = lba >> 16;
	h2d_fis->Lba3 = lba >> 24;

	h2d_fis->CountLow  = sector_sz & 0xFF;
	h2d_fis->CountHigh = (sector_sz >> 8) & 0xFF;

	kSATAPort->Ci = (1 << slot);

	while ((kSATAPort->Ci & (1 << slot)) != 0)
	{
		if (kSATAPort->Is & HBA_ERR_TFE)
		{
			kcout << "AHCI: Task file disk error, setting global err...\r";

			err_global_get() = Kernel::kErrorUnrecoverableDisk;

			return;
		}
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
