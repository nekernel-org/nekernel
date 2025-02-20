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

#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/LPC.h>

#include <Mod/ATA/ATA.h>
#include <Mod/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#ifdef __AHCI__

#define kHBAErrTaskFile (1 << 30)
#define kHBAPxCmdST		(0x0001)
#define kHBAPxCmdFre	(0x0010)
#define kHBAPxCmdFR		(0x4000)
#define kHBAPxCmdCR		(0x8000)

#define kSataLBAMode (1 << 6)

#define kAhciSRBsy (0x80)
#define kAhciSRDrq (0x08)

#define kAhciPortCnt (0x20)

using namespace NeOS;

enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06,
	kSATABar5		= 0x24,
};

STATIC PCI::Device kPCIDevice;
STATIC HbaMem*			 kSATA			 = nullptr;
STATIC SizeT kSATAIndex			 = 0UL;
STATIC Lba kHighestLBA = 0UL;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output(UInt64 lba, UInt8* buffer, SizeT sector_sz, SizeT size_buffer) noexcept;

STATIC Int32 drv_find_cmd_slot(HbaPort* port) noexcept;

STATIC Void drv_calculate_disk_geometry() noexcept;

STATIC Void drv_calculate_disk_geometry() noexcept
{
	kHighestLBA = 0UL;

	const auto kSzIdent = 512;

	UInt8 identify_data[kSzIdent] = {0};

	rt_set_memory(identify_data, 0, kSzIdent);

	drv_std_input_output<NO, YES, YES>(0, identify_data, 0, kSzIdent);

	kHighestLBA = (identify_data[61] << 16) | identify_data[60];

	kout << "Disk Size: " << number(drv_get_size()) << endl;
	kout << "Highest LBA: " << number(kHighestLBA) << endl;
}

/// @brief Initializes an AHCI disk.
/// @param pi the amount of kSATA that have been detected.
/// @return if the disk was successfully initialized or not.
Boolean drv_std_init(UInt16& pi)
{
	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT device_index = 0; device_index < NE_BUS_COUNT; ++device_index)
	{
		kPCIDevice = iterator[device_index].Leak(); // Leak device.

		if (kPCIDevice.Subclass() == kSATASubClass &&
			kPCIDevice.ProgIf() == kSATAProgIfAHCI)
		{
			HbaMem* mem_ahci = (HbaMem*)kPCIDevice.Bar(kSATABar5);

			kPCIDevice.EnableMmio((UInt32)(UIntPtr)mem_ahci);	  // Enable the memory index_byte/o for this ahci device.
			kPCIDevice.BecomeBusMaster((UInt32)(UIntPtr)mem_ahci); // Become bus master for this ahci device, so that we can control it.

			UInt32 ports_implemented = mem_ahci->Pi;
			UInt16 ahci_index		   = 0;

			const UInt16 kMaxPortsImplemented = kAhciPortCnt;
			const UInt32 kSATASignature		= 0x00000101;
			const UInt8  kAhciPresent			= 0x03;
			const UInt8  kAhciIPMActive		= 0x01;

			Boolean detected = false;

			while (ahci_index < kMaxPortsImplemented)
			{
				if (ports_implemented)
				{
					UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature && det == kAhciPresent && ipm == kAhciIPMActive)
					{
						kout << "SATA port found.\r";

						kSATAIndex = ahci_index;
						kSATA		 = mem_ahci;

						drv_calculate_disk_geometry();

						detected = YES;

						pi = ports_implemented;

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

Boolean drv_std_detected(Void)
{
	return kPCIDevice.DeviceId() != (UShort)PCI::PciConfigKind::Invalid;
}

Void drv_std_write(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer)
{
	drv_std_input_output<YES, YES, NO>(lba, (UInt8*)buffer, sector_sz, size_buffer);
}

Void drv_std_read(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer)
{
	drv_std_input_output<NO, YES, NO>(lba, (UInt8*)buffer, sector_sz, size_buffer);
}

STATIC Int32 drv_find_cmd_slot(HbaPort* port) noexcept
{
	if (port == nullptr)
		return ~0;

	UInt32 slots = (kSATA->Ports[kSATAIndex].Sact | kSATA->Ports[kSATAIndex].Ci);

	for (Int32 i = 0; i < kAhciPortCnt; ++i)
	{
		if ((slots & 1) == 0)
			return i;

		slots >>= 1;
	}

	return ~0;
}

BOOL kAHCICommandIssued = NO;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output(UInt64 lba, UInt8* buffer, SizeT sector_sz, SizeT size_buffer) noexcept
{
	auto slot = 0L;

	slot = drv_find_cmd_slot(&kSATA->Ports[kSATAIndex]);

	if (slot == ~0)
		return;

	HbaCmdHeader* command_header = ((HbaCmdHeader*)((UInt64)kSATA->Ports[kSATAIndex].Clb));

	command_header += slot;

	MUST_PASS(command_header);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(UInt32);
	command_header->Write = Write;
	command_header->Prdtl = 1;

	HbaCmdTbl* command_table = (HbaCmdTbl*)((UInt64)command_header->Ctba);

	MUST_PASS(command_table);

	auto buffer_phys = HAL::hal_get_phys_address(buffer);

	command_table->Prdt[0].Dba	= ((UInt32)(UInt64)buffer_phys & __UINT32_MAX__);
	command_table->Prdt[0].Dbau = (((UInt64)(buffer_phys) >> 32) & __UINT32_MAX__);
	command_table->Prdt[0].Dbc	= ((size_buffer) - 1);
	command_table->Prdt[0].Ie	= YES;

	FisRegH2D* h2d_fis = (FisRegH2D*)((UInt64)&command_table->Cfis);

	h2d_fis->FisType   = kFISTypeRegH2D;
	h2d_fis->CmdOrCtrl = CommandOrCTRL;
	h2d_fis->Command   = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

	if (Identify)
		h2d_fis->Command = kAHCICmdIdentify;

	h2d_fis->Lba0 = (lba) & 0xFF;
	h2d_fis->Lba1 = (lba >> 8) & 0xFF;
	h2d_fis->Lba2 = (lba >> 16) & 0xFF;

	h2d_fis->Device = kSataLBAMode;

	h2d_fis->Lba3 = (lba >> 24) & 0xFF;
	h2d_fis->Lba4 = (lba >> 32) & 0xFF;
	h2d_fis->Lba5 = (lba >> 40) & 0xFF;

	h2d_fis->CountLow  = (size_buffer) & 0xFF;
	h2d_fis->CountHigh = (size_buffer >> 8) & 0xFF;

	while ((kSATA->Ports[kSATAIndex].Tfd & (kAhciSRBsy | kAhciSRDrq)))
	{
		kout << "Waiting for slot to be ready\r";
	}

	kSATA->Ports[kSATAIndex].Ci = (1 << slot);
	kAHCICommandIssued			  = YES;

	while (kSATA->Ports[kSATAIndex].Ci & (1 << slot))
	{
		if (kSATA->Is & kHBAErrTaskFile) // check for task file error.
		{
			ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");
		}
	}

	if (kSATA->Is & kHBAErrTaskFile) // check for task file error.
	{
		ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");
	}
}

/***
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
SizeT drv_get_sector_count()
{
	return kHighestLBA;
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
SizeT drv_get_size()
{
	return drv_get_sector_count() * kAHCISectorSize;
}

#endif // ifdef __AHCI__