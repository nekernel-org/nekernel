/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss Corporation, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI.cc
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 */

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/LPC.h>

#include <FirmwareKit/EPM.h>

#include <Mod/ATA/ATA.h>
#include <Mod/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#include <StorageKit/AHCI.h>

#define kHBAErrTaskFile (1 << 30)
#define kHBAPxCmdST		(0x0001)
#define kHBAPxCmdFre	(0x0010)
#define kHBAPxCmdFR		(0x4000)
#define kHBAPxCmdCR		(0x8000)

#define kSATALBAMode (1 << 6)

#define kSATASRBsy (0x80)
#define kSATASRDrq (0x08)

#define kHBABohcBiosOwned (1 << 0)
#define kHBABohcOSOwned	  (1 << 1)

#define kSATAPortCnt (0x20)

#define kSATASig   (0x00000101)
#define kSATAPISig (0xEB140101)

#define kSATAProgIfAHCI (0x01)
#define kSATASubClass	(0x06)
#define kSATABar5		(0x24)

using namespace NeOS;

STATIC PCI::Device kSATADev;
STATIC HbaMem* kSATAHba;
STATIC Lba	   kSATASectorCount		 = 0UL;
STATIC UInt16  kSATAIndex			 = 0U;
STATIC Char	   kCurrentDiskModel[50] = {"UNKNOWN AHCI DRIVE"};
STATIC UInt16  kSATAPortsImplemented = 0U;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output(UInt64 lba, UInt8* buffer, SizeT sector_sz, SizeT size_buffer) noexcept;

STATIC Int32 drv_find_cmd_slot(HbaPort* port) noexcept;

STATIC Void drv_compute_disk_ahci() noexcept;

STATIC Void drv_compute_disk_ahci() noexcept
{
	kSATASectorCount = 0UL;

	const UInt16 kSzIdent = 512;

	UInt8 identify_data[kSzIdent] = {0};

	rt_set_memory(identify_data, 0, kSzIdent);

	drv_std_input_output<NO, YES, YES>(0, identify_data, kAHCISectorSize, kSzIdent);

	kSATASectorCount = (identify_data[61] << 16) | identify_data[60];

	kout << "Disk Model: " << kCurrentDiskModel << kendl;
	kout << "Disk Size: " << number(drv_get_size()) << kendl;
	kout << "Disk Sector Count: " << number(kSATASectorCount) << kendl;
}

STATIC Int32 drv_find_cmd_slot(HbaPort* port) noexcept
{
	UInt32 slots = (port->Sact | port->Ci);

	for (Int32 i = 0; i < kSATAPortCnt; ++i)
	{
		if ((slots & 1) == 0)
			return i;

		slots >>= 1;
	}

	return ~0;
}

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output(UInt64 lba, UInt8* buffer, SizeT sector_sz, SizeT size_buffer) noexcept
{
	UIntPtr slot = 0UL;

	slot = drv_find_cmd_slot(&kSATAHba->Ports[kSATAIndex]);

	if (slot == ~0)
		return;

	volatile HbaCmdHeader* command_header = ((HbaCmdHeader*)(((UInt64)kSATAHba->Ports[kSATAIndex].Clb)));

	MUST_PASS(command_header);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(UInt32);
	command_header->Write = Write;
	command_header->Prdtl = (UInt16)((size_buffer - 1) >> 4) + 1;

	HbaCmdTbl* command_table = (HbaCmdTbl*)((VoidPtr)((UInt64)command_header->Ctba));
	rt_set_memory(command_table, 0, sizeof(HbaCmdTbl) + (command_header->Prdtl - 1) * sizeof(HbaPrdtEntry));

	MUST_PASS(command_table);

	UIntPtr buffer_phys = HAL::hal_get_phys_address(buffer);

	UInt16 prd_i = 0;

	for (; prd_i < (command_header->Prdtl - 1); prd_i++)
	{
		command_table->Prdt[prd_i].Dbc	= ((size_buffer / command_header->Prdtl - 1) - 1);
		command_table->Prdt[prd_i].Dba	= ((UInt32)(UInt64)buffer_phys + (prd_i * command_table->Prdt[prd_i].Dbc));
		command_table->Prdt[prd_i].Dbau = (((UInt64)(buffer_phys) >> 32) + (prd_i * command_table->Prdt[prd_i].Dbc));
		command_table->Prdt[prd_i].Ie	= YES;
	}

	command_table->Prdt[prd_i].Dba	= ((UInt32)(UInt64)buffer_phys + (prd_i * command_table->Prdt[prd_i].Dbc));
	command_table->Prdt[prd_i].Dbau = (((UInt64)(buffer_phys) >> 32) + (prd_i * command_table->Prdt[prd_i].Dbc));
	command_table->Prdt[prd_i].Dbc	= ((size_buffer / command_header->Prdtl - 1) - 1);
	command_table->Prdt[prd_i].Ie	= YES;

	FisRegH2D* h2d_fis = (FisRegH2D*)(&command_table->Cfis);

	h2d_fis->FisType   = kFISTypeRegH2D;
	h2d_fis->CmdOrCtrl = CommandOrCTRL;
	h2d_fis->Command   = Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx;

	if (Identify)
		h2d_fis->Command = kAHCICmdIdentify;

	h2d_fis->Lba0 = (lba)&0xFF;
	h2d_fis->Lba1 = (lba >> 8) & 0xFF;
	h2d_fis->Lba2 = (lba >> 16) & 0xFF;

	h2d_fis->Device = Identify ? 0U : kSATALBAMode;

	h2d_fis->Lba3 = (lba >> 24) & 0xFF;
	h2d_fis->Lba4 = (lba >> 32) & 0xFF;
	h2d_fis->Lba5 = (lba >> 40) & 0xFF;

	h2d_fis->CountLow  = (size_buffer)&0xFF;
	h2d_fis->CountHigh = (size_buffer >> 8) & 0xFF;

	while (kSATAHba->Ports[kSATAIndex].Tfd & (kSATASRBsy | kSATASRDrq))
		;

	kSATAHba->Ports[kSATAIndex].Ci = (1 << slot);

	while (YES)
	{
		if (!(kSATAHba->Ports[kSATAIndex].Ci & (1 << slot)))
			break;

		if (kSATAHba->Is & kHBAErrTaskFile)
		{
			err_global_get() = kErrorDiskIsCorrupted;
			return;
		}
	}

	if (kSATAHba->Is & kHBAErrTaskFile)
	{
		err_global_get() = kErrorDiskIsCorrupted;
		return;
	}
}

/***
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
SizeT drv_get_sector_count_ahci()
{
	return kSATASectorCount;
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
SizeT drv_get_size_ahci()
{
	return drv_get_sector_count() * kAHCISectorSize;
}

/// @brief Initializes an AHCI disk.
/// @param pi the amount of ports that have been detected.
/// @return if the disk was successfully initialized or not.
STATIC Bool drv_std_init_ahci(UInt16& pi, BOOL atapi)
{
	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT device_index = 0; device_index < NE_BUS_COUNT; ++device_index)
	{
		kSATADev = iterator[device_index].Leak(); // Leak device.

		if (kSATADev.Subclass() == kSATASubClass &&
			kSATADev.ProgIf() == kSATAProgIfAHCI)
		{
			HbaMem* mem_ahci = (HbaMem*)kSATADev.Bar(kSATABar5);

			kSATADev.EnableMmio((UIntPtr)mem_ahci);
			kSATADev.BecomeBusMaster((UIntPtr)mem_ahci);

			UInt32 ports_implemented = mem_ahci->Pi;
			UInt16 ahci_index		 = 0;

			kSATAPortsImplemented = ports_implemented;
			pi					  = kSATAPortsImplemented;

			const UInt16 kMaxPortsImplemented = kSATAPortCnt;
			const UInt32 kSATASignature		  = kSATASig;
			const UInt32 kSATAPISignature	  = kSATAPISig;
			const UInt8	 kSATAPresent		  = 0x03;
			const UInt8	 kSATAIPMActive		  = 0x01;

			kSATAHba = mem_ahci;

			while (ports_implemented)
			{
				UInt8 ipm = (mem_ahci->Ports[ahci_index].Ssts >> 8) & 0x0F;
				UInt8 det = mem_ahci->Ports[ahci_index].Ssts & 0x0F;

				if (det != kSATAPresent || ipm != kSATAIPMActive)
					continue;

				if (mem_ahci->Ports[ahci_index].Sig == kSATASignature)
				{
					kout << "Detect: /dev/sat" << number(ahci_index) << kendl;

					kSATAIndex = ahci_index;
					kSATAHba   = mem_ahci;

					if (kSATAHba->Bohc & kHBABohcBiosOwned)
					{
						kSATAHba->Bohc |= kHBABohcOSOwned;

						while (kSATAHba->Bohc & kHBABohcBiosOwned)
						{
						}
					}

					kSATAHba->Ports[kSATAIndex].Cmd |= kHBAPxCmdFre;
					kSATAHba->Ports[kSATAIndex].Cmd |= kHBAPxCmdST;

					drv_compute_disk_ahci();

					break;
				}
				else if (atapi && kSATAPISignature == mem_ahci->Ports[ahci_index].Sig)
				{
					kout << "Detect: /dev/atp" << number(ahci_index) << kendl;

					kSATAIndex = ahci_index;
					kSATAHba   = mem_ahci;

					kSATAHba->Ports[kSATAIndex].Cmd |= kHBAPxCmdFre;
					kSATAHba->Ports[kSATAIndex].Cmd |= kHBAPxCmdST;

					drv_compute_disk_ahci();

					break;
				}

				ports_implemented >>= 1;
				++ahci_index;
			}

			return YES;
		}
	}

	return NO;
}

Bool drv_std_detected_ahci()
{
	return kSATADev.DeviceId() != (UShort)PCI::PciConfigKind::Invalid && kSATADev.Bar(kSATABar5) != 0;
}

namespace NeOS
{
	UInt16 sk_init_ahci_device(BOOL atapi)
	{
		UInt16 pi = 0;
		return drv_std_init_ahci(pi, atapi);

		return pi;
	}

	ErrorOr<AHCIDeviceInterface> sk_acquire_ahci_device(Int32 drv_index)
	{
		if (!drv_std_detected_ahci())
			return ErrorOr<AHCIDeviceInterface>(kErrorDisk);

		AHCIDeviceInterface device([](IDeviceObject<MountpointInterface*>* self, MountpointInterface* mnt) -> void {
		AHCIDeviceInterface* dev = (AHCIDeviceInterface*)self;
		
		auto disk = mnt->GetAddressOf(dev->GetIndex());
		drv_std_input_output<YES, YES, NO>(disk->fPacket.fPacketLba, (UInt8*)disk->fPacket.fPacketContent, kAHCISectorSize, disk->fPacket.fPacketSize); },
								   [](IDeviceObject<MountpointInterface*>* self, MountpointInterface* mnt) -> void {
									   AHCIDeviceInterface* dev = (AHCIDeviceInterface*)self;

									   auto disk = mnt->GetAddressOf(dev->GetIndex());
									   drv_std_input_output<NO, YES, NO>(disk->fPacket.fPacketLba, (UInt8*)disk->fPacket.fPacketContent, kAHCISectorSize, disk->fPacket.fPacketSize);
								   },
								   nullptr);

		device.SetPi(kSATAPortsImplemented);
		device.SetIndex(drv_index);

		return ErrorOr<AHCIDeviceInterface>(device);
	}
} // namespace NeOS

#ifdef __AHCI__

Void drv_std_write(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer)
{
	drv_std_input_output<YES, YES, NO>(lba, (UInt8*)buffer, sector_sz, size_buffer);
}

Void drv_std_read(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer)
{
	drv_std_input_output<NO, YES, NO>(lba, (UInt8*)buffer, sector_sz, size_buffer);
}

Bool drv_std_init(UInt16& pi)
{
	return drv_std_init_ahci(pi, NO);
}

Bool drv_std_detected(Void)
{
	return drv_std_detected_ahci();
}

/***
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
SizeT drv_get_sector_count()
{
	return drv_get_sector_count_ahci();
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
SizeT drv_get_size()
{
	return drv_get_size_ahci();
}

#endif // ifdef __AHCI__
