/* -------------------------------------------

	Copyright (C) 2024-2025, DeVrije-Aker Corporation, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI.cc
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @Copyright (C) 2024, DeVrije-Aker, all rights reserved.
 *
 */

#include "HALKit/AMD64/Processor.h"
#include "KernelKit/DebugOutput.h"
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/LPC.h>

#include <Mod/ATA/ATA.h>
#include <Mod/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#ifdef __AHCI__

#define kHBAErrTaskFile (1 << 30)
#define kHBAPxCmdST		0x0001
#define kHBAPxCmdFre	0x0010
#define kHBAPxCmdFR		0x4000
#define kHBAPxCmdCR		0x8000

#define kSataLBAMode (1 << 6)

#define kAhciSRBsy (0x80)
#define kAhciSRDrq (0x08)

#define kAhciPortCnt 32

enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06,
	kSATABar5		= 0x24,
};

STATIC Kernel::PCI::Device kPCIDevice;
STATIC HbaMem*			   kSATA		   = nullptr;
STATIC Kernel::SizeT kSATAPortIdx		   = 0UL;
STATIC Kernel::Lba kCurrentDiskSectorCount = 0UL;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept;

static Kernel::Int32 drv_find_cmd_slot(HbaPort* port) noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept
{
	kCurrentDiskSectorCount = 0UL;

	Kernel::UInt8 identify_data[kib_cast(4)] = {0};

	drv_std_input_output<NO, YES, YES>(0, identify_data, 0, kib_cast(8));

	kCurrentDiskSectorCount = (identify_data[61] << 16) | identify_data[60];

	kout << "Disk Size: " << Kernel::number(drv_get_size()) << endl;
	kout << "Highest LBA: " << Kernel::number(kCurrentDiskSectorCount) << endl;
}

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of kSATA that have been detected.
/// @return if the disk was successfully initialized or not.
Kernel::Boolean drv_std_init(Kernel::UInt16& PortsImplemented)
{
	using namespace Kernel;

	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT device_index = 0; device_index < NE_BUS_COUNT; ++device_index)
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
					kout << "Port is implemented.\r";

					Kernel::UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					Kernel::UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature && det == kAhciPresent && ipm == kAhciIPMActive)
					{
						kout << "Port is SATA.\r";

						kSATAPortIdx = ahci_index;
						kSATA		 = mem_ahci;

						kSATA->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdFre;
						kSATA->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdST;

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
	drv_std_input_output<YES, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer);
}

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	drv_std_input_output<NO, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer);
}

static Kernel::Int32 drv_find_cmd_slot(HbaPort* port) noexcept
{
	if (port == nullptr)
		return -1;

	Kernel::UInt32 slots = (kSATA->Ports[kSATAPortIdx].Sact | kSATA->Ports[kSATAPortIdx].Ci);

	for (Kernel::Int32 i = 0; i < 32; i++)
	{
		if ((slots & 1) == 0)
			return i;

		slots >>= 1;
	}

	return -1;
}

BOOL kAHCICommandIssued = NO;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept
{
	auto slot = 0L;

	slot = drv_find_cmd_slot(&kSATA->Ports[kSATAPortIdx]);

	if (slot == -1)
		return;

	volatile HbaCmdHeader* command_header = ((volatile HbaCmdHeader*)((Kernel::UInt64)kSATA->Ports[kSATAPortIdx].Clb));

	command_header += slot;

	MUST_PASS(command_header);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	command_header->Write = Write;
	command_header->Prdtl = 2;

	volatile HbaCmdTbl* command_table = (volatile HbaCmdTbl*)((Kernel::UInt64)command_header->Ctba);

	MUST_PASS(command_table);

	auto buffer_phys = Kernel::HAL::hal_get_phys_address(buffer);

	command_table->Prdt[0].Dba	= ((Kernel::UInt32)(Kernel::UInt64)buffer_phys & 0xFFFFFFFF);
	command_table->Prdt[0].Dbau = (((Kernel::UInt64)(buffer_phys) >> 32) & 0xFFFFFFFF);
	command_table->Prdt[0].Dbc	= ((size_buffer / 2) - 1);
	command_table->Prdt[0].Ie	= YES;

	command_table->Prdt[1].Dba	= ((Kernel::UInt32)(Kernel::UInt64)(buffer_phys + ((size_buffer / 2) - 1)) & 0xFFFFFFFF);
	command_table->Prdt[1].Dbau = (((Kernel::UInt64)(buffer_phys + ((size_buffer / 2) - 1)) >> 32) & 0xFFFFFFFF);
	command_table->Prdt[1].Dbc	= ((size_buffer / 2) - 1);
	command_table->Prdt[1].Ie	= YES;

	volatile FisRegH2D* h2d_fis = (volatile FisRegH2D*)((Kernel::UInt64)&command_table->Cfis);

	h2d_fis->FisType   = kFISTypeRegH2D;
	h2d_fis->CmdOrCtrl = CommandOrCTRL;
	h2d_fis->Command   = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

	if (Identify)
		h2d_fis->Command = kAHCICmdIdentify;

	h2d_fis->Lba0 = (lba & 0xFF);
	h2d_fis->Lba1 = (lba >> 8) & 0xFF;
	h2d_fis->Lba2 = (lba >> 16) & 0xFF;
	h2d_fis->Lba3 = (lba >> 24) & 0xFF;
	h2d_fis->Lba4 = (lba >> 32) & 0xFF;
	h2d_fis->Lba5 = (lba >> 40) & 0xFF;

	h2d_fis->Device = kSataLBAMode;

	h2d_fis->CountLow  = (sector_sz - 1) & 0xFF;
	h2d_fis->CountHigh = ((sector_sz - 1) >> 8) & 0xFF;

	while ((kSATA->Ports[kSATAPortIdx].Tfd & (kAhciSRBsy | kAhciSRDrq)))
	{
		kout << "Waiting for slot to be ready\r";
	}

	kSATA->Ports[kSATAPortIdx].Ci = 1 << slot;
	kAHCICommandIssued			  = YES;

	while (kSATA->Ports[kSATAPortIdx].Ci & (1 << slot))
	{
		kout << Kernel::hex_number(command_header->Prdtl) << endl;
	
		if (kSATA->Is & kHBAErrTaskFile) // check for task file error.
		{
			Kernel::ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");
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