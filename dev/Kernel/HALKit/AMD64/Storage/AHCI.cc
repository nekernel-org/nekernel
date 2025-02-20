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

STATIC NeOS::PCI::Device kPCIDevice;
STATIC HbaMem*			 kSATA			 = nullptr;
STATIC NeOS::SizeT kSATAPortIdx			 = 0UL;
STATIC NeOS::Lba kCurrentDiskSectorCount = 0UL;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static NeOS::Void drv_std_input_output(NeOS::UInt64 lba, NeOS::UInt8* buffer, NeOS::SizeT sector_sz, NeOS::SizeT size_buffer) noexcept;

static NeOS::Int32 drv_find_cmd_slot(HbaPort* port) noexcept;

static NeOS::Void drv_calculate_disk_geometry() noexcept;

static NeOS::Void drv_calculate_disk_geometry() noexcept
{
	kCurrentDiskSectorCount = 0UL;

	NeOS::UInt8 identify_data[kib_cast(4)] = {0};

	drv_std_input_output<NO, YES, YES>(0, identify_data, 0, kib_cast(8));

	kCurrentDiskSectorCount = (identify_data[61] << 16) | identify_data[60];

	kout << "Disk Size: " << NeOS::number(drv_get_size()) << endl;
	kout << "Highest LBA: " << NeOS::number(kCurrentDiskSectorCount) << endl;
}

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of kSATA that have been detected.
/// @return if the disk was successfully initialized or not.
NeOS::Boolean drv_std_init(NeOS::UInt16& PortsImplemented)
{
	using namespace NeOS;

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

			NeOS::UInt32 ports_implemented = mem_ahci->Pi;
			NeOS::UInt16 ahci_index		   = 0;

			const NeOS::UInt16 kMaxPortsImplemented = kAhciPortCnt;
			const NeOS::UInt32 kSATASignature		= 0x00000101;
			const NeOS::UInt8  kAhciPresent			= 0x03;
			const NeOS::UInt8  kAhciIPMActive		= 0x01;

			NeOS::Boolean detected = false;

			while (ahci_index < kMaxPortsImplemented)
			{
				if (ports_implemented)
				{
					NeOS::UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
					NeOS::UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

					if (mem_ahci->Ports[ahci_index].Sig == kSATASignature && det == kAhciPresent && ipm == kAhciIPMActive)
					{
						kout << "SATA port found.\r";

						kSATAPortIdx = ahci_index;
						kSATA		 = mem_ahci;

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

NeOS::Boolean drv_std_detected(NeOS::Void)
{
	return kPCIDevice.DeviceId() != (NeOS::UShort)NeOS::PCI::PciConfigKind::Invalid;
}

NeOS::Void drv_std_write(NeOS::UInt64 lba, NeOS::Char* buffer, NeOS::SizeT sector_sz, NeOS::SizeT size_buffer)
{
	drv_std_input_output<YES, YES, NO>(lba, (NeOS::UInt8*)buffer, sector_sz, size_buffer);
}

NeOS::Void drv_std_read(NeOS::UInt64 lba, NeOS::Char* buffer, NeOS::SizeT sector_sz, NeOS::SizeT size_buffer)
{
	drv_std_input_output<NO, YES, NO>(lba, (NeOS::UInt8*)buffer, sector_sz, size_buffer);
}

static NeOS::Int32 drv_find_cmd_slot(HbaPort* port) noexcept
{
	if (port == nullptr)
		return -1;

	NeOS::UInt32 slots = (kSATA->Ports[kSATAPortIdx].Sact | kSATA->Ports[kSATAPortIdx].Ci);

	for (NeOS::Int32 i = 0; i < kAhciPortCnt; ++i)
	{
		if ((slots & 1) == 0)
			return i;

		slots >>= 1;
	}

	return -1;
}

BOOL kAHCICommandIssued = NO;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
static NeOS::Void drv_std_input_output(NeOS::UInt64 lba, NeOS::UInt8* buffer, NeOS::SizeT sector_sz, NeOS::SizeT size_buffer) noexcept
{
	kSATA->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdFre;
	kSATA->Ports[kSATAPortIdx].Cmd |= kHBAPxCmdST;

	auto slot = 0L;

	slot = drv_find_cmd_slot(&kSATA->Ports[kSATAPortIdx]);

	if (slot == -1)
		return;

	volatile HbaCmdHeader* command_header = ((volatile HbaCmdHeader*)((NeOS::UInt64)kSATA->Ports[kSATAPortIdx].Clb));

	command_header += slot;

	MUST_PASS(command_header);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(NeOS::UInt32);
	command_header->Write = Write;
	command_header->Prdtl = 2;

	volatile HbaCmdTbl* command_table = (volatile HbaCmdTbl*)((NeOS::UInt64)command_header->Ctba);

	MUST_PASS(command_table);

	auto buffer_phys = NeOS::HAL::hal_get_phys_address(buffer);

	command_table->Prdt[0].Dba	= ((NeOS::UInt32)(NeOS::UInt64)buffer_phys);
	command_table->Prdt[0].Dbau = (((NeOS::UInt64)(buffer_phys) >> 32));
	command_table->Prdt[0].Dbc	= ((size_buffer / 2) - 1);
	command_table->Prdt[0].Ie	= YES;

	command_table->Prdt[1].Dba	= ((NeOS::UInt32)(NeOS::UInt64)(buffer_phys + ((size_buffer / 2) - 1)));
	command_table->Prdt[1].Dbau = (((NeOS::UInt64)(buffer_phys + ((size_buffer / 2) - 1)) >> 32));
	command_table->Prdt[1].Dbc	= ((size_buffer / 2) - 1);
	command_table->Prdt[1].Ie	= YES;

	volatile FisRegH2D* h2d_fis = (volatile FisRegH2D*)((NeOS::UInt64)&command_table->Cfis);

	h2d_fis->FisType   = kFISTypeRegH2D;
	h2d_fis->CmdOrCtrl = CommandOrCTRL;
	h2d_fis->Command   = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

	if (Identify)
		h2d_fis->Command = kAHCICmdIdentify;

	h2d_fis->Lba0 = (lba);
	h2d_fis->Lba1 = (lba >> 8);
	h2d_fis->Lba2 = (lba >> 16);

	h2d_fis->Device = kSataLBAMode;

	h2d_fis->Lba3 = (lba >> 24);
	h2d_fis->Lba4 = (lba >> 32);
	h2d_fis->Lba5 = (lba >> 40);

	h2d_fis->CountLow  = (size_buffer) & 0xFF;
	h2d_fis->CountHigh = ((size_buffer) >> 8) & 0xFF;

	while ((kSATA->Ports[kSATAPortIdx].Tfd & (kAhciSRBsy | kAhciSRDrq)))
	{
		kout << "Waiting for slot to be ready\r";
	}

	kSATA->Ports[kSATAPortIdx].Ci = 1 << slot;
	kAHCICommandIssued			  = YES;

	while (kSATA->Ports[kSATAPortIdx].Ci & (1 << slot))
	{
		if (kSATA->Is & kHBAErrTaskFile) // check for task file error.
		{
			NeOS::ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");
		}
	}

	kSATA->Ports[kSATAPortIdx].Cmd &= ~kHBAPxCmdFre;
	kSATA->Ports[kSATAPortIdx].Cmd &= ~kHBAPxCmdST;

	if (kSATA->Is & kHBAErrTaskFile) // check for task file error.
	{
		NeOS::ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR, "AHCI Read disk failure, faulty component.");
	}
}

/***
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
NeOS::SizeT drv_get_sector_count()
{
	return kCurrentDiskSectorCount;
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
NeOS::SizeT drv_get_size()
{
	return drv_get_sector_count() * kAHCISectorSize;
}

#endif // ifdef __AHCI__