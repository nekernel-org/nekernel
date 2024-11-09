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

#include <Modules/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#ifdef __AHCI__
#define kMaxAhciPoll (100000U)

#define kCmdOrCtrlCmd 1
#define kCmdOrCtrlCtrl 0

enum
{
	kSATAProgIfAHCI = 0x01,
	kSATASubClass	= 0x06,
	kSATABar5		= 0x24,
};

STATIC Kernel::PCI::Device kAhciDevice;
STATIC HbaPort*			   kAhciPort = nullptr;

/// @brief Initializes an AHCI disk.
/// @param PortsImplemented the amount of kAhciPort that have been detected.
/// @return
Kernel::Boolean drv_std_init(Kernel::UInt16& PortsImplemented)
{
	using namespace Kernel;

	PCI::Iterator iterator(Types::PciDeviceKind::MassStorageController);

	for (SizeT devIndex = 0; devIndex < ZKA_BUS_COUNT; ++devIndex)
	{
		// if SATA and then interface is AHCI...
		if (iterator[devIndex].Leak().Subclass() == kSATASubClass &&
			iterator[devIndex].Leak().ProgIf() == kSATAProgIfAHCI)
		{
			iterator[devIndex].Leak().EnableMmio(0x24);		 // Enable the memory index_byte/o for this ahci device.
			iterator[devIndex].Leak().BecomeBusMaster(0x24); // Become bus master for this ahci device, so that we can control it.

			kAhciDevice = iterator[devIndex].Leak(); // And then leak the reference.

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

						mem_ahci->Ghc |= (1 << kAhciGHC_AE);

						kAhciPort = &mem_ahci->Ports[ahci_index];

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
	STATIC Kernel::Boolean kSlotIsUsed = No;

	Kernel::LockDelegate<kMaxAhciPoll> lock(&kSlotIsUsed);

	if (kSlotIsUsed)
		return;

	// Slot is now used, OS uses slot 0, driver must not use slot 0.

	kSlotIsUsed = Yes;

	Kernel::Int64 free_slot = 0L;

	// Prepare command header.

	HbaCmdHeader* cmd_header = (HbaCmdHeader*)(Kernel::UIntPtr)kAhciPort->Clb;
	cmd_header += free_slot;

	// Read operation/set entries count.

	cmd_header->Write = No;
	cmd_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1; // PRDT entries count

	// Prepare command table.

	HbaCmdTbl* cmd_tbl = (HbaCmdTbl*)(Kernel::UIntPtr)cmd_header->Ctba;
	Kernel::rt_set_memory(cmd_tbl, 0, sizeof(HbaCmdTbl));

	Kernel::UInt64 size		  = size * kAHCISectorSize;
	Kernel::Int64  index_byte = 0L;

	for (index_byte = 0; index_byte < (cmd_header->Prdtl - 1); ++index_byte)
	{
		cmd_tbl->PrdtEntries[index_byte].Dba		  = (Kernel::UInt32)(Kernel::UIntPtr)buffer;
		cmd_tbl->PrdtEntries[index_byte].Dbc		  = (kib_cast(8)) - 1; // 8KB buffer size
		cmd_tbl->PrdtEntries[index_byte].InterruptBit = 1;				// Interrupt on completion

		size -= kib_cast(8);
		buffer += kib_cast(4); // Move the buffer pointer forward
	}

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
	kAhciPort->Ci = 1 << free_slot;

	// 7. Wait for the command to complete (simple spinlock, no need for an object here)
	while (Yes)
	{
		if (!(kAhciPort->Ci & (1 << free_slot)))
		{
			break; // Command has completed
		}
		else if (kAhciPort->Is & (1 << 30))
		{
			return; // Error in task file
		}
	}

	kSlotIsUsed = No;
}

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_sz, Kernel::SizeT size_buffer)
{
	STATIC Kernel::Boolean kSlotIsUsed = No;

	if (kSlotIsUsed)
		return;

	kSlotIsUsed = Yes;

	Kernel::Int64 free_slot = 0L;

	// Prepare command header.

	HbaCmdHeader* cmd_header = (HbaCmdHeader*)(Kernel::UIntPtr)kAhciPort->Clb;
	cmd_header += free_slot;

	// Read operation/set entries count.

	cmd_header->Write = Yes;
	cmd_header->Prdtl = (Kernel::UInt16)((size_buffer - 1) >> 4) + 1; // PRDT entries count, put in low of prdt.

	// Prepare command table.

	HbaCmdTbl* cmd_tbl = (HbaCmdTbl*)(Kernel::UIntPtr)cmd_header->Ctba;
	Kernel::rt_set_memory(cmd_tbl, 0, sizeof(HbaCmdTbl));

	Kernel::UInt64 size		  = size * kAHCISectorSize;
	Kernel::Int64  index_byte = 0L;

	for (index_byte = 0; index_byte < (cmd_header->Prdtl - 1); ++index_byte)
	{
		cmd_tbl->PrdtEntries[index_byte].Dba		  = (Kernel::UInt32)(Kernel::UIntPtr)buffer;
		cmd_tbl->PrdtEntries[index_byte].Dbc		  = (kib_cast(8)) - 1; // 8KB buffer size
		cmd_tbl->PrdtEntries[index_byte].InterruptBit = 1;				// Interrupt on completion

		size -= kib_cast(8);
		buffer += kib_cast(4); // Move the buffer pointer forward
	}

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
	kAhciPort->Ci = 1 << free_slot;

	// 7. Wait for the command to complete (simple spinlock, no need for an object here)
	while (Yes)
	{
		if (!(kAhciPort->Ci & (1 << free_slot)))
		{
			break; // Command has completed
		}
		else if (kAhciPort->Is & (1 << 30))
		{
			kcout << "Error in task file. (AHCI Drv)\r";
			Kernel::ke_stop(RUNTIME_CHECK_UNEXCPECTED);
			return; // Error in task file
		}
	}

	kSlotIsUsed = No;
}

/***
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
Kernel::SizeT drv_get_sector_count()
{
	return 0;
}

/// @brief Get the drive size.
/// @return Disk size in bytes.
Kernel::SizeT drv_get_size()
{
	return drv_get_sector_count() * kAHCISectorSize;
}

#endif // __AHCI__
