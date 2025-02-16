/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI.cc
 * @author Amlal EL Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
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

#define kHBAErrTaskFile (1 << 30)
#define kHBACmdGhc		(1 << 31)
#define kHBACmdAE		(0x80000000)
#define kHBAPxCmdST		(0x0001)
#define kHBAPxCmdFre	(0x0010)
#define kHBAPxCmdFR		(0x4000)
#define kHBAPxCmdCR		(0x8000)

#define kSATALBAMode (1 << 6)

#define kSATASRBsy (0x80)
#define kSATASRDrq (0x08)

#define kSATAPortCnt (0x20)

#define kSATAProgIfAHCI (0x01)
#define kSATASubClass	(0x06)
#define kSATABar5		(0x24)

STATIC Kernel::PCI::Device kDevice;
STATIC HbaMem*			   kSATA		   = nullptr;
STATIC Kernel::SizeT kSATAPortIdx		   = 0UL;
STATIC Kernel::Lba kCurrentDiskSectorCount = 0UL;
STATIC Kernel::Char kCurrentDiskModel[50]  = {"UNKNOWN SATA DRIVE"};

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drvi_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept;

static Kernel::Int32 drvi_find_cmd_slot(HbaPort* port) noexcept;

static Kernel::Void drvi_calculate_disk_geometry() noexcept;

static Kernel::Void drvi_calculate_disk_geometry() noexcept
{
	kCurrentDiskSectorCount = 0UL;

	Kernel::UInt8* identify_data = new Kernel::UInt8[512];

	drvi_std_input_output<NO, YES, YES>(0, identify_data, 0, 512);

	kCurrentDiskSectorCount = (identify_data[61] << 16) | identify_data[60];

	for (Kernel::Int32 i = 0; i < 20; i++)
	{
		kCurrentDiskModel[i * 2]	 = identify_data[27 + i] >> 8;
		kCurrentDiskModel[i * 2 + 1] = identify_data[27 + i] & 0xFF;
	}

	kCurrentDiskModel[40] = '\0';

	kout << "Drive Model: " << kCurrentDiskModel << endl;

	kout << "Disk Size: " << Kernel::number(drv_get_size()) << endl;
	kout << "Highest Disk LBA: " << Kernel::number(kCurrentDiskSectorCount) << endl;

	delete [] identify_data;
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
		kDevice = iterator[device_index].Leak(); // And then leak the reference.

		// if SATA and then interface is AHCI...
		if (kDevice.Subclass() == kSATASubClass &&
			kDevice.ProgIf() == kSATAProgIfAHCI)
		{
			HbaMem* mem_ahci = (HbaMem*)kDevice.Bar(kSATABar5);

			kDevice.EnableMmio(kDevice.Bar(kSATABar5));		 // Enable the memory index_byte/o for this ahci device.
			kDevice.BecomeBusMaster(kDevice.Bar(kSATABar5)); // Become bus master for this ahci device, so that we can control it.

			Kernel::UInt32 ports_implemented = mem_ahci->Pi;
			Kernel::UInt16 ahci_index		 = 0;

			const Kernel::UInt16 kMaxPortsImplemented = kSATAPortCnt;
			const Kernel::UInt32 kSATASignature		  = 0x00000101;
			const Kernel::UInt8	 kSATAPresent		  = 0x03;
			const Kernel::UInt8	 kSATAIPMActive		  = 0x01;

			while (ahci_index < kMaxPortsImplemented)
			{
				if (ports_implemented)
				{
					Kernel::UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					Kernel::UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature && det == kSATAPresent && ipm == kSATAIPMActive &&
						(mem_ahci->Ports[ahci_index].Ssts & 0xF))
					{
						kout << "Port is implemented as SATA.\r";

						kSATAPortIdx = ahci_index;
						kSATA		 = mem_ahci;

						drvi_calculate_disk_geometry();

						return YES;
					}
				}

				ports_implemented >>= 1;
				++ahci_index;
			}
		}
	}

	return No;
}

Kernel::Boolean drv_std_detected(Kernel::Void)
{
	return kDevice.DeviceId() != 0xFFFF && kCurrentDiskSectorCount > 0;
}

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	drvi_std_input_output<YES, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer);
}

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	drvi_std_input_output<NO, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer);
}

static Kernel::Int32 drvi_find_cmd_slot(HbaPort* port) noexcept
{
	Kernel::UInt32 slots = port->Ci;

	for (Kernel::Int32 i = 0; i < kSATAPortCnt; i++)
	{
		if ((slots & 1) == 0)
			return i;

		slots >>= 1;
	}

	return -1;
}

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drvi_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept
{
	const Kernel::SizeT slot = drvi_find_cmd_slot(&kSATA->Ports[kSATAPortIdx]);

	if (slot == -1)
		return;

	if (size_buffer > mib_cast(4))
		Kernel::ke_panic(RUNTIME_CHECK_FAILED, "AHCI only supports < 4mib DMA transfers per PRD.");

	if (!Write)
		Kernel::rt_set_memory(buffer, 0, size_buffer);

	HbaCmdHeader* ATTRIBUTE(aligned(1024)) command_header = ((HbaCmdHeader*)((Kernel::UInt64)(kSATA->Ports[kSATAPortIdx].Clb)));

	command_header->Cfl			 = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	command_header->Write		 = Write;
	command_header->Prdtl		 = 1;
	command_header->Atapi		 = 0;
	command_header->Prefetchable = 0;

	HbaCmdTbl* command_table = ((HbaCmdTbl*)((Kernel::UInt64)(command_header->Ctba)));

	Kernel::rt_set_memory(command_table, 0, sizeof(HbaCmdTbl));

	MUST_PASS(command_table);

	auto phys_dma_buf = (Kernel::UIntPtr)Kernel::HAL::hal_get_phys_address(buffer);

	command_table->Prdt[0].Dbau = (((Kernel::UInt64)phys_dma_buf >> 32));
	command_table->Prdt[0].Dba	= ((Kernel::UInt32)(Kernel::UInt64)phys_dma_buf);
	command_table->Prdt[0].Dbc	= ((size_buffer)-1);
	command_table->Prdt[0].Ie	= 0;

	FisRegH2D* h2d_fis = (FisRegH2D*)((Kernel::UInt64)&command_table->Cfis);

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

	h2d_fis->Device = kSATALBAMode;

	// 28-bit LBA mode, fis is done being configured.

	h2d_fis->CountLow  = sector_sz & 0xFF;
	h2d_fis->CountHigh = (sector_sz >> 8) & 0xFF;

	if (kSATA->Is & kHBAErrTaskFile)
		Kernel::ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");

	kSATA->Ports[kSATAPortIdx].Ie  = 0;

	kSATA->Ports[kSATAPortIdx].Cmd = kHBAPxCmdFR | kHBAPxCmdST;
	kSATA->Ports[kSATAPortIdx].Ci = (1 << slot);

	while (!drv_is_ready())
		;
}

BOOL drv_is_ready(void)
{
	return kSATA->Ports[kSATAPortIdx].Ci == 0;
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
