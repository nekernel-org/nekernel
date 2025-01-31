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
 * @Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.
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
STATIC HbaMem* kSATAPort				   = nullptr;
STATIC Kernel::SizeT kSATAPortIdx		   = 0UL;
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
	kcout << "Highest Disk LBA: " << Kernel::number(kCurrentDiskSectorCount) << endl;
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
			kPCIDevice.EnableMmio(kSATABar5);	   // Enable the memory index_byte/o for this ahci device.
			kPCIDevice.BecomeBusMaster(kSATABar5); // Become bus master for this ahci device, so that we can control it.

			HbaMem* mem_ahci = (HbaMem*)kPCIDevice.Bar(kSATABar5);

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
						kcout << "Port is implemented as SATA.\r";

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

						kSATAPort->Ports[kSATAPortIdx].Clb	= kAHCIBaseAddress + (kSATAPortIdx << 10);
						kSATAPort->Ports[kSATAPortIdx].Clbu = 0;

						rt_set_memory((VoidPtr)((UIntPtr)kSATAPort->Ports[kSATAPortIdx].Clb), 0, 1024);

						kSATAPort->Ports[kSATAPortIdx].Fb  = kAHCIBaseAddress + (32 << 10) + (kSATAPortIdx << 8);
						kSATAPort->Ports[kSATAPortIdx].Fbu = 0;

						rt_set_memory((VoidPtr)((UIntPtr)kSATAPort->Ports[kSATAPortIdx].Fb), 0, 256);

						HbaCmdHeader* cmd_header = (HbaCmdHeader*)((UIntPtr)kSATAPort->Ports[kSATAPortIdx].Clb);

						for (Int32 i = 0; i < 32; i++)
						{
							cmd_header[i].Prdtl = 8;

							cmd_header[i].Ctba	= kAHCIBaseAddress + (40 << 10) + (kSATAPortIdx << 13) + (i << 8);
							cmd_header[i].Ctbau = 0;

							rt_set_memory((VoidPtr)(UIntPtr)cmd_header[i].Ctba, 0, 256);
						}

						while (kSATAPort->Ports[kSATAPortIdx].Cmd & kHBAPxCmdCR)
							;

						kSATAPort->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdFre;
						kSATAPort->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdST;

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

	kSATAPort->Ports[kSATAPortIdx].Is = -1;

	auto slot = 0L;

	slot = drv_find_cmd_slot(&kSATAPort->Ports[kSATAPortIdx]);

	if (slot == -1)
		return;

	volatile HbaCmdHeader* command_header = ((volatile HbaCmdHeader*)((Kernel::UInt64)kSATAPort->Ports[kSATAPortIdx].Clb + kSATAPort->Ports[kSATAPortIdx].Clbu));

	command_header += slot;

	MUST_PASS(command_header);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	command_header->Write = Write;
	command_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1;

	volatile HbaCmdTbl* command_table = (volatile HbaCmdTbl*)((Kernel::UInt64)command_header->Ctba + command_header->Ctbau);

	MUST_PASS(command_table);

	for (Kernel::SizeT i = 0; i < (command_header->Prdtl - 1); i++)
	{
		command_table->PrdtEntries[i].Dba		   = ((Kernel::UInt32)(Kernel::UInt64)buffer & 0xFFFFFFFF);
		command_table->PrdtEntries[i].Dbau		   = (((Kernel::UInt64)buffer >> 32) & 0xFFFFFFFF);
		command_table->PrdtEntries[i].Dbc		   = (size_buffer - 1);
		command_table->PrdtEntries[i].InterruptBit = YES;

		size_buffer -= 16;
		buffer += kib_cast(4);
	}

	volatile FisRegH2D* h2d_fis = (volatile FisRegH2D*)((Kernel::UInt64)command_table->Cfis);

	h2d_fis->FisType = kFISTypeRegH2D;

	h2d_fis->CmdOrCtrl = CommandOrCTRL;

	h2d_fis->Command = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

	if (Identify)
		h2d_fis->Command = kAHCICmdIdentify;

	h2d_fis->Lba0 = lba;
	h2d_fis->Lba1 = lba >> 8;
	h2d_fis->Lba2 = lba >> 16;
	h2d_fis->Lba3 = lba >> 24;

	h2d_fis->Device = kSataLBAMode;

	// 28-bit LBA mode, fis is done being configured.

	h2d_fis->CountLow  = sector_sz & 0xFF;
	h2d_fis->CountHigh = (sector_sz >> 8) & 0xFF;

	while ((kSATAPort->Ports[kSATAPortIdx].Tfd & (kAhciSRBsy | kAhciSRDrq)))
	{
		kcout << "Waiting for slot to be ready...\r\n";
	}

	if (kSATAPort->Is & kHBAErrTaskFile)
		Kernel::ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");

	// send fis/cmdtbl/cmdhdr.
	kSATAPort->Ports[kSATAPortIdx].Ci |= 1 << slot;

	while (kSATAPort->Ports[kSATAPortIdx].Ci & (1 << slot))
		;

	while ((kSATAPort->Ports[kSATAPortIdx].Tfd & (kAhciSRBsy | kAhciSRDrq)))
	{
		kcout << "Waiting for slot to be ready...\r\n";
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
