/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI.cc
 * @author Amlal EL Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.
 *
 */

#include "HALKit/AMD64/Processor.h"
#include "KernelKit/DebugOutput.h"
#include "KernelKit/Heap.h"
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
STATIC HbaMem*			   kSATAPort	   = nullptr;
STATIC Kernel::SizeT kSATAPortIdx		   = 0UL;
STATIC Kernel::Lba kCurrentDiskSectorCount = 0UL;
STATIC Kernel::Char kModel[41]			   = {0};

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static Kernel::Void drv_std_input_output(Kernel::UInt64 lba, Kernel::UInt8* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer) noexcept;

static Kernel::Int32 drv_find_cmd_slot(HbaPort* port) noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept;

static Kernel::Void drv_calculate_disk_geometry() noexcept
{
	kCurrentDiskSectorCount = 0UL;

	Kernel::UInt8 __attribute__((aligned(4096))) identify_data[kib_cast(4)] = {};

	drv_std_input_output<NO, YES, YES>(0, identify_data, 0, kib_cast(8));

	if (!(identify_data[83] & (1 << 10)))
	{
		kCurrentDiskSectorCount = identify_data[61] << 16;
		kCurrentDiskSectorCount |= identify_data[60];
	}
	else
	{
		kCurrentDiskSectorCount = (identify_data[103] << 48) | (identify_data[102] << 32) | (identify_data[101] << 16) | (identify_data[100]);
	}

	for (Kernel::Int32 i = 0; i < 40; i += 2)
	{
		Kernel::Char temp		  = identify_data[54 + i];
		identify_data[54 + i]	  = identify_data[54 + i + 1];
		identify_data[54 + i + 1] = temp;
	}

	Kernel::rt_copy_memory((Kernel::Char*)(identify_data + 54), kModel, 40);
	kModel[40] = '\0';

	kcout << "SATA Model: " << kModel << "\r";

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
			HbaMem* mem_ahci = (HbaMem*)kPCIDevice.Bar(kSATABar5);

			kPCIDevice.EnableMmio(kSATABar5);	   // Enable the memory index_byte/o for this ahci device.
			kPCIDevice.BecomeBusMaster(kSATABar5); // Become bus master for this ahci device, so that we can control it.

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

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature && det == 3 && ipm == 1)
					{
						kcout << "Port is SATA.\r";

						kSATAPortIdx = ahci_index;
						kSATAPort	 = mem_ahci;

						kSATAPort->Ports[kSATAPortIdx].Cmd &= ~kHBAPxCmdST;
						kSATAPort->Ports[kSATAPortIdx].Cmd &= ~kHBAPxCmdFre;

						while (YES)
						{
							if (kSATAPort->Ports[kSATAPortIdx].Cmd & kHBAPxCmdFR)
								continue;
							if (kSATAPort->Ports[kSATAPortIdx].Cmd & kHBAPxCmdCR)
								continue;

							break;
						}

						const auto kAHCIBaseAddress = mib_cast(4);

						kSATAPort->Ports[kSATAPortIdx].Clb = kAHCIBaseAddress + (kSATAPortIdx << 10);

						rt_set_memory((VoidPtr)((UIntPtr)kSATAPort->Ports[kSATAPortIdx].Clb), 0, 1024);

						kSATAPort->Ports[kSATAPortIdx].Fb  = kAHCIBaseAddress + (32 << 10) + (kSATAPortIdx << 8);
						kSATAPort->Ports[kSATAPortIdx].Fbu = 0;

						rt_set_memory((VoidPtr)((UIntPtr)kSATAPort->Ports[kSATAPortIdx].Fb), 0, 256);

						HbaCmdHeader* cmd_header = (HbaCmdHeader*)((UIntPtr)kSATAPort->Ports[kSATAPortIdx].Clb);

						for (Int32 i = 0; i < kMaxPortsImplemented; i++)
						{
							cmd_header[i].Prdtl = 1;
							cmd_header[i].Ctba	= kAHCIBaseAddress + (40 << 10) + (kSATAPortIdx << 13) + (i << 8);

							rt_set_memory((VoidPtr)(UIntPtr)cmd_header[i].Ctba, 0, 256);
						}

						while (kSATAPort->Ports[kSATAPortIdx].Cmd & kHBAPxCmdCR)
							;

						kSATAPort->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdFre;
						kSATAPort->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdST;

						// Enable AHCI Mode FIRST
						kSATAPort->Ghc |= (1 >> 31);

						drv_calculate_disk_geometry();

						return YES;
					}
				}

				ports_implemented >>= 1;
				++ahci_index;
			}
		}
	}

	return NO;
}

Kernel::Boolean drv_std_detected(Kernel::Void)
{
	return kPCIDevice.DeviceId() != 0xFFFF;
}

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	drv_std_input_output<YES, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer / sector_sz);
}

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	drv_std_input_output<NO, YES, NO>(lba, (Kernel::UInt8*)buffer, sector_sz, size_buffer / sector_sz);
}

static Kernel::Int32 drv_find_cmd_slot(HbaPort* port) noexcept
{
	if (port == nullptr)
		return -1;

	Kernel::UInt32 slots = (kSATAPort->Ports[kSATAPortIdx].Sact | kSATAPort->Ports[kSATAPortIdx].Ci);

	for (Kernel::Int32 i = 0; i < ((kSATAPort->Cap & 0x1F) + 1); i++)
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

	auto slot = 0L;

	slot = drv_find_cmd_slot(&kSATAPort->Ports[kSATAPortIdx]);

	if (slot == -1)
		return;

	kcout << "Reading AHCI disk...\r";

	volatile HbaCmdHeader* command_header = ((volatile HbaCmdHeader*)((Kernel::UInt64)kSATAPort->Ports[kSATAPortIdx].Clb + slot * sizeof(HbaCmdHeader)));

	Kernel::rt_set_memory((void*)command_header, 0, sizeof(HbaCmdTbl));

	MUST_PASS(command_header);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	command_header->Write = Write;
	command_header->Prdtl = 1;
	command_header->Ctba  = (Kernel::UIntPtr)Kernel::mm_new_heap(sizeof(HbaCmdTbl), YES, NO);

	volatile HbaCmdTbl* command_table = (volatile HbaCmdTbl*)((Kernel::UInt64)command_header->Ctba);

	Kernel::rt_set_memory((void*)command_table, 0, sizeof(HbaCmdTbl));

	MUST_PASS(command_table);

	command_table->PrdtEntries->Dba			 = (Kernel::UInt64)buffer;
	command_table->PrdtEntries->Dbc			 = (size_buffer * sector_sz) - 1;
	command_table->PrdtEntries->InterruptBit = YES;
	kcout << "PRDT Entry 0 - Dba: " << Kernel::hex_number(command_table->PrdtEntries->Dba) << endl;
	kcout << "PRDT Entry 0 - Dbc: " << Kernel::hex_number(command_table->PrdtEntries->Dbc) << endl;

	volatile FisRegH2D* h2d_fis = (volatile FisRegH2D*)(command_table->Cfis);

	Kernel::rt_set_memory((void*)h2d_fis, 0, sizeof(FisRegH2D));

	h2d_fis->FisType = kFISTypeRegH2D;

	h2d_fis->CmdOrCtrl = CommandOrCTRL;

	h2d_fis->Command = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

	if (Identify)
		h2d_fis->Command = kAHCICmdIdentify;

	h2d_fis->Lba0 = (Kernel::UInt32)(lba) & 0xFF;
	h2d_fis->Lba1 = (Kernel::UInt8)(lba >> 8) & 0xFF;
	h2d_fis->Lba2 = (Kernel::UInt8)(lba >> 16) & 0xFF;

	h2d_fis->Lba3 = (Kernel::UInt8)(lba >> 24) & 0xFF;
	h2d_fis->Lba4 = (Kernel::UInt8)(lba >> 32) & 0xFF;
	h2d_fis->Lba5 = (Kernel::UInt8)(lba >> 40) & 0xFF;

	h2d_fis->Device = kSataLBAMode;

	h2d_fis->CountLow  = sector_sz & 0xFF;
	h2d_fis->CountHigh = (sector_sz >> 8) & 0xFF;

	while ((kSATAPort->Ports[kSATAPortIdx].Tfd & (kAhciSRBsy | kAhciSRDrq)))
	{
		kcout << "waiting for slot to be ready\r\n";
	}

	kSATAPort->Ports[kSATAPortIdx].Ci = 1 << slot;

	while (kSATAPort->Ports[kSATAPortIdx].Ci & (1 << slot))
	{
		Kernel::UInt32 tfd = kSATAPort->Ports[kSATAPortIdx].Tfd;
		kcout << "AHCI TFD: " << Kernel::number(tfd) << endl;
	}

	kcout << "Last Command Sent: " << (int)Kernel::number(h2d_fis->Command) << endl;

	Kernel::mm_delete_heap((Kernel::VoidPtr)command_header->Ctba);
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
