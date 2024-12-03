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

#include <Modules/ATA/ATA.h>
#include <Modules/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

#ifdef __AHCI__

#define AHCI_START_ADDRESS mib_cast(4) // 4M

#define HBA_ERR_TFE	  (1 << 30)
#define HBA_PxCMD_ST  0x0001
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_FR  0x4000
#define HBA_PxCMD_CR  0x8000

#define AHCI_LBA_MODE (1 << 6)

#define kMaxAhciPoll (100000U)

#define kCmdOrCtrlCmd  1
#define kCmdOrCtrlCtrl 0

#define kAhciSRBsy 0x80
#define kAhciSRDrq 0x08

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
	kCurrentDiskSectorCount = 0UL;
	kcout << "Highest AHCI LBA (48-bit): " << Kernel::number(kCurrentDiskSectorCount) << endl;
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
			kAhciDevice.EnableMmio(0x24);	   // Enable the memory index_byte/o for this ahci device.
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

						kAhciPort->Cmd &= ~HBA_PxCMD_FRE;

						// Clear FRE (bit4)
						kAhciPort->Cmd &= ~HBA_PxCMD_ST;

						// Wait until FR (bit14), CR (bit15) are cleared
						while (YES)
						{
							if (kAhciPort->Cmd & HBA_PxCMD_CR)
								continue;

							if (kAhciPort->Cmd & HBA_PxCMD_FR)
								continue;
							break;
						}

						// when it's stopped.

						// do in-between

						kAhciPort->Clb	= AHCI_START_ADDRESS + (ahci_index << 10);
						kAhciPort->Clbu = 0;
						rt_set_memory((void*)(kAhciPort->Clb), 0, 1024);

						// FIS offset: 32K+256*ahci_index
						// FIS entry size = 256 bytes per port
						kAhciPort->Fb  = AHCI_START_ADDRESS + (32 << 10) + (ahci_index << 8);
						kAhciPort->Fbu = 0;
						rt_set_memory((void*)(kAhciPort->Fb), 0, 256);

						// Command table offset: 40K + 8K*ahci_index
						// Command table size = 256*32 = 8K per port
						HbaCmdHeader* cmdheader = (HbaCmdHeader*)(kAhciPort->Clb);

						for (int i = 0; i < 32; i++)
						{
							cmdheader[i].Prdtl = 8; // 8 prdt entries per command table
													// 256 bytes per command table, 64+16+48+16*8
							// Command table offset: 40K + 8K*ahci_index + cmdheader_index*256
							cmdheader[i].Ctba  = AHCI_START_ADDRESS + (40 << 10) + (ahci_index << 13) + (i << 8);
							cmdheader[i].Ctbau = 0;

							rt_set_memory((void*)cmdheader[i].Ctba, 0, 256);
						}

						// when it's starting

						// check for bits again, to start it again.
						while (YES)
						{
							if (kAhciPort->Cmd & HBA_PxCMD_FR)
								continue;

							break;
						}

						kAhciPort->Cmd |= HBA_PxCMD_FRE;
						kAhciPort->Cmd |= HBA_PxCMD_ST;

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

Kernel::Void drv_std_write(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_cnt, Kernel::SizeT size_buffer)
{
}

Kernel::Void drv_std_read(Kernel::UInt64 lba, Kernel::Char* buffer, Kernel::SizeT sector_cnt, Kernel::SizeT size_buffer)
{
	kAhciPort->Is		 = -1;
	int			   port	 = 0;
	Kernel::UInt32 slots = (kAhciPort->Sact | kAhciPort->Ci);

	for (; port < slots; ++port)
	{
		if ((slots & 1) == 0)
			break;

		slots >>= 1;
	}

	HbaCmdHeader* cmd_hdr = (HbaCmdHeader*)(kAhciPort->Clb);

	cmd_hdr += port;
	cmd_hdr->Cfl   = sizeof(FisRegH2D) / sizeof(Kernel::UInt32);
	cmd_hdr->Write = NO;
	cmd_hdr->Prdtl = (Kernel::UInt16)((sector_cnt - 1) >> 4) + 1;

	HbaCmdTbl* cmd_tbl = (HbaCmdTbl*)(cmd_hdr->Ctba);
	Kernel::rt_set_memory(cmd_tbl, 0, (cmd_hdr->Prdtl - 1) * sizeof(HbaPrdtEntry));

	int i = 0;

	for (int i = 0; i < cmd_hdr->Prdtl - 1; i++)
	{
		cmd_tbl->PrdtEntries[i].Dba			 = (Kernel::UInt32)(Kernel::UInt64)buffer;
		cmd_tbl->PrdtEntries[i].Dbau		 = (Kernel::UInt32)((Kernel::UInt64)(buffer) >> 32);
		cmd_tbl->PrdtEntries[i].Dbc			 = size_buffer - 1; // 8K bytes (this value should always be set to 1 less than the actual value)
		cmd_tbl->PrdtEntries[i].InterruptBit = 1;
	}

	cmd_tbl->PrdtEntries[i].Dba			 = (Kernel::UInt32)(Kernel::UInt64)buffer;
	cmd_tbl->PrdtEntries[i].Dbau		 = (Kernel::UInt32)((Kernel::UInt64)(buffer) >> 32);
	cmd_tbl->PrdtEntries[i].Dbc			 = size_buffer - 1; // 8K bytes (this value should always be set to 1 less than the actual value)
	cmd_tbl->PrdtEntries[i].InterruptBit = 1;

	FisRegH2D* cmd_fis = (FisRegH2D*)(&cmd_tbl->Cfis);

	cmd_fis->FisType   = kFISTypeRegH2D;
	cmd_fis->CmdOrCtrl = YES; // Command
	cmd_fis->Command   = kAHCICmdReadDmaEx;

	cmd_fis->Lba0	= (Kernel::UInt8)(Kernel::UInt32)lba & 0xFF;
	cmd_fis->Lba1	= (Kernel::UInt8)((Kernel::UInt32)lba >> 8);
	cmd_fis->Lba2	= (Kernel::UInt8)((Kernel::UInt32)lba >> 16);
	cmd_fis->Device = AHCI_LBA_MODE; // LBA mode

	cmd_fis->Lba3 = (Kernel::UInt8)((Kernel::UInt32)lba >> 24);
	cmd_fis->Lba4 = (Kernel::UInt8)(lba >> 32);
	cmd_fis->Lba5 = (Kernel::UInt8)((lba >> 32) >> 8);

	cmd_fis->CountLow  = sector_cnt & 0xFF;
	cmd_fis->CountHigh = (sector_cnt >> 8) & 0xFF;

	Kernel::UInt64 spin = 0UL;

	// The below loop waits until the port is no longer busy before issuing a new command
	while ((kAhciPort->Tfd & (kAhciSRBsy | kAhciSRDrq)) && spin < kMaxAhciPoll)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kcout << "Port is hung\r";
		return;
	}

	kAhciPort->Ci = 1 << port; // Issue command

	// Wait for completion
	while (YES)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit
		// in the PxIS port field as well (1 << 5)
		if ((kAhciPort->Ci & (1 << port)) == 0)
			break;
		if (kAhciPort->Is & HBA_ERR_TFE) // Task file error
		{
			kcout << ("Read disk error\r");
			return;
		}
	}

	// Check again for the last time.
	if (kAhciPort->Is & HBA_ERR_TFE) // task file error status
	{
		kcout << ("Read disk error\r");
		*buffer = 0;
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

#endif // ifdef __AHCI__
