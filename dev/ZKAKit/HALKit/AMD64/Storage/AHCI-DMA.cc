/* -------------------------------------------

	Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI.cc
 * @author EL Mahrouss Logic (amlalelmahrouss@icloud.com)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @Copyright (C) 2024, EL Mahrouss Logic, all rights reserved.
 *
 */

#include <Modules/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#ifdef __AHCI__
#define kMaxAhciPoll (100000U)

#define kCmdOrCtrlCmd  1
#define kCmdOrCtrlCtrl 0

enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06,
	kSATABar5		= 0x24,
};

STATIC Kernel::PCI::Device kAhciDevice;
STATIC HbaPort* kAhciPort				   = nullptr;
STATIC Kernel::Lba kCurrentDiskSectorCount = 0UL;

Kernel::Void drv_calculate_disk_geometry()
{
	// Slot is now used, OS uses slot 0, driver must not use slot 0.

	// Prepare command header.

	HbaCmdHeader* cmd_header = (HbaCmdHeader*)(Kernel::UIntPtr)kAhciPort->Clb;

	// Read operation/set entries count.

	Kernel::UInt16 identify_data[256] = {0};
	Kernel::UInt64 size				  = 512 - 1; // Adjust size for 512 bytes of data (Dbc should be 511)

	cmd_header->Cfl	  = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	cmd_header->Prdtl = 1; // 1 PRDT entry

	// Prepare command table.
	HbaCmdTbl* cmd_tbl = (HbaCmdTbl*)(Kernel::UIntPtr)cmd_header->Ctba;

	// First PRD entry
	cmd_tbl->PrdtEntries[0].Dba			 = (Kernel::UInt32)(Kernel::UIntPtr)identify_data;
	cmd_tbl->PrdtEntries[0].Dbc			 = size; // Byte count (511 for 512 bytes of data)
	cmd_tbl->PrdtEntries[0].InterruptBit = 1;	 // Set interrupt flag

	// 5. Prepare the command FIS (Frame Information Structure)
	FisRegH2D* cmd_fis = (FisRegH2D*)(cmd_tbl->Cfis);
	cmd_fis->FisType   = kFISTypeRegH2D;
	cmd_fis->CmdOrCtrl = kCmdOrCtrlCmd; // Command
	cmd_fis->Command   = kAHCICmdIdentify;

	// 6. Issue the command by writing to the port's command issue register (CI)
	kAhciPort->Ci = 1; // Issue command slot 0

	// Polling loop for command completion
	while (kAhciPort->Ci)
	{
		// Check for errors in the Task File Data register (PxTFD)
		if (kAhciPort->Tfd & (1 << 0))
		{ // Error bit
			kcout << "Error in task file, can't get disk geometry." << endl;
			Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
		}

		// If the device is busy, wait
		if (kAhciPort->Tfd & (1 << 7))
		{
			kcout << "Device is busy, waiting..." << endl;
			continue;
		}

		// If command completes, clear the PxCI register
		if (kAhciPort->Is & (1 << 30))
		{ // If an interrupt occurred (device ready)
			kcout << "Command completed successfully." << endl;
			break;
		}
	}

	// Retrieve the max LBA value
	kCurrentDiskSectorCount = 0UL;
	kCurrentDiskSectorCount |= identify_data[100];
	kCurrentDiskSectorCount |= identify_data[101] << 16;
	kCurrentDiskSectorCount |= identify_data[102] << 32;
	kCurrentDiskSectorCount |= identify_data[103] << 48;

	kcout << "Max LBA: " << Kernel::number(kCurrentDiskSectorCount) << endl;

	BREAK_POINT();
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
			kAhciDevice.EnableMmio(0x24);		 // Enable the memory index_byte/o for this ahci device.
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

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	// Slot is now used, OS uses slot 0, driver must not use slot 0.

	// Prepare command header.

	HbaCmdHeader* cmd_header = (HbaCmdHeader*)(Kernel::UIntPtr)kAhciPort->Clb;

	// Read operation/set entries count.

	cmd_header->Write = No;
	cmd_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1; // PRDT entries count

	// Prepare command table.

	HbaCmdTbl* cmd_tbl = (HbaCmdTbl*)(Kernel::UIntPtr)cmd_header->Ctba;
	Kernel::rt_set_memory(cmd_tbl, 0, sizeof(HbaCmdTbl));

	Kernel::UInt64 size		  = size * kAHCISectorSize;
	Kernel::Int64  index_byte = 0L;

	cmd_tbl->PrdtEntries[index_byte].Dba		  = (Kernel::UInt32)(Kernel::UIntPtr)buffer;
	cmd_tbl->PrdtEntries[index_byte].Dbc		  = size; // 8KB buffer size
	cmd_tbl->PrdtEntries[index_byte].InterruptBit = 1;	  // Interrupt on completion

	// Last PRDT entry
	cmd_tbl->PrdtEntries[index_byte].Dba		  = (Kernel::UInt32)(Kernel::UIntPtr)buffer;
	cmd_tbl->PrdtEntries[index_byte].Dbc		  = size - 1; // Byte count left
	cmd_tbl->PrdtEntries[index_byte].InterruptBit = 1;

	// 5. Prepare the command FIS (Frame Information Structure)
	FisRegH2D* cmd_fis = (FisRegH2D*)(&cmd_tbl->Cfis);
	Kernel::rt_set_memory(cmd_fis, 0, sizeof(FisRegH2D));

	cmd_fis->FisType   = kFISTypeRegH2D;
	cmd_fis->CmdOrCtrl = kCmdOrCtrlCmd; // Command
	cmd_fis->Command   = kAHCICmdReadDmaEx;

	cmd_fis->Lba0	= (Kernel::UInt8)lba;
	cmd_fis->Lba1	= (Kernel::UInt8)(lba >> 8);
	cmd_fis->Lba2	= (Kernel::UInt8)(lba >> 16);
	cmd_fis->Device = 1 << 6; // LBA mode

	cmd_fis->Lba3 = (Kernel::UInt8)(lba >> 24);
	cmd_fis->Lba4 = (Kernel::UInt8)(lba >> 32);
	cmd_fis->Lba5 = (Kernel::UInt8)(lba >> 40);

	cmd_fis->CountLow  = size & 0xFF;
	cmd_fis->CountHigh = (size >> 8) & 0xFF;

	// 6. Issue the command by writing to the port's command issue register (CI)
	kAhciPort->Ci = 1;

	// 7. Wait for the command to complete (simple spinlock, no need for an object here)
	while (Yes)
	{
		if (!(kAhciPort->Ci & 1))
		{
			break; // Command has completed
		}
		else if (kAhciPort->Is & (1 << 30))
		{
			return; // Error in task file
		}

		kcout << Kernel::number((kAhciPort->Ci & 1)) << endl;
	}
}

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	// Prepare command header.

	HbaCmdHeader* cmd_header = (HbaCmdHeader*)(Kernel::UIntPtr)kAhciPort->Clb;

	// Read operation/set entries count.

	cmd_header->Write = Yes;
	cmd_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1; // PRDT entries count, put in low of prdt.

	// Prepare command table.

	HbaCmdTbl* cmd_tbl = (HbaCmdTbl*)(Kernel::UIntPtr)cmd_header->Ctba;
	Kernel::rt_set_memory(cmd_tbl, 0, sizeof(HbaCmdTbl));

	Kernel::UInt64 size		  = size * kAHCISectorSize;
	Kernel::Int64  index_byte = 0L;

	cmd_tbl->PrdtEntries[index_byte].Dba		  = (Kernel::UInt32)(Kernel::UIntPtr)buffer;
	cmd_tbl->PrdtEntries[index_byte].Dbc		  = size; // 8KB buffer size
	cmd_tbl->PrdtEntries[index_byte].InterruptBit = 1;	  // Interrupt on completion

	// Last PRDT entry
	cmd_tbl->PrdtEntries[index_byte].Dba		  = (Kernel::UInt32)(Kernel::UIntPtr)buffer;
	cmd_tbl->PrdtEntries[index_byte].Dbc		  = size - 1; // Byte count left
	cmd_tbl->PrdtEntries[index_byte].InterruptBit = 1;

	// 5. Prepare the command FIS (Frame Information Structure)
	FisRegH2D* cmd_fis = (FisRegH2D*)(&cmd_tbl->Cfis);
	Kernel::rt_set_memory(cmd_fis, 0, sizeof(FisRegH2D));

	cmd_fis->FisType   = kFISTypeRegH2D;
	cmd_fis->CmdOrCtrl = kCmdOrCtrlCmd; // Command
	cmd_fis->Command   = kAHCICmdReadDmaEx;

	cmd_fis->Lba0	= (Kernel::UInt8)lba;
	cmd_fis->Lba1	= (Kernel::UInt8)(lba >> 8);
	cmd_fis->Lba2	= (Kernel::UInt8)(lba >> 16);
	cmd_fis->Device = 1 << 6; // LBA mode

	cmd_fis->Lba3 = (Kernel::UInt8)(lba >> 24);
	cmd_fis->Lba4 = (Kernel::UInt8)(lba >> 32);
	cmd_fis->Lba5 = (Kernel::UInt8)(lba >> 40);

	cmd_fis->CountLow  = size & 0xFF;
	cmd_fis->CountHigh = (size >> 8) & 0xFF;

	// 6. Issue the command by writing to the port's command issue register (CI)
	kAhciPort->Ci = 1;

	// 7. Wait for the command to complete (simple spinlock, no need for an object here)
	while (Yes)
	{
		if (!(kAhciPort->Ci & 1))
		{
			break; // Command has completed
		}
		else if (kAhciPort->Is & (1 << 30))
		{
			kcout << "Error in task file. (AHCI Drv)\r";
			Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
			return; // Error in task file
		}

		kcout << Kernel::number((kAhciPort->Ci & 1)) << endl;
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
	return drv_get_sector_count() * kAHCISectorSize;
}

#endif // __AHCI__
