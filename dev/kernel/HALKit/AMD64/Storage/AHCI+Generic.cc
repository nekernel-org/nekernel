/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss Corporation, all rights reserved.

------------------------------------------- */

/**
 * @file AHCI+Generic.cc
 * @author Amlal El Mahrouss (amlal@nekernel.org)
 * @brief AHCI driver.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 */

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/DriveMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/KPC.h>
#include <FirmwareKit/EPM.h>
#include <StorageKit/AHCI.h>
#include <modules/ATA/ATA.h>
#include <modules/AHCI/AHCI.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Utils.h>
#include <KernelKit/LockDelegate.h>

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

using namespace Kernel;

STATIC PCI::Device kSATADev;
STATIC HbaMemRef   kSATAHba;
STATIC Lba		   kSATASectorCount		 = 0UL;
STATIC UInt16	   kSATAIndex			 = 0U;
STATIC Char		   kCurrentDiskModel[50] = {"UNKNOWN AHCI DRIVE"};
STATIC UInt16	   kSATAPortsImplemented = 0U;

template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output_ahci(UInt64 lba, UInt8* buffer, SizeT sector_sz, SizeT size_buffer) noexcept;

STATIC Int32 drv_find_cmd_slot_ahci(HbaPort* port) noexcept;

STATIC Void drv_compute_disk_ahci() noexcept;

STATIC Void drv_compute_disk_ahci() noexcept
{
	kSATASectorCount = 0UL;

	/// Normally 512 bytes, but add an additional 512 bytes to make 1 KIB.
	const UInt16 kSzIdent = kib_cast(1);

	/// Push it to the stack
	static UInt8 identify_data[kSzIdent] ATTRIBUTE(aligned(4096)) = {0};

	HAL::mm_map_page((void*)mib_cast(1), (void*)mib_cast(1), HAL::kMMFlagsWr);

	rt_set_memory(identify_data, 0, kSzIdent);

	/// Send AHCI command for identification.
	drv_std_input_output_ahci<NO, YES, YES>(0, identify_data, kAHCISectorSize, kSzIdent);

	/// Extract 48-bit LBA.

	UInt64 lba48_sectors = 0;
	lba48_sectors |= (UInt64)identify_data[100];
    lba48_sectors |= (UInt64)identify_data[101] << 16;
    lba48_sectors |= (UInt64)identify_data[102] << 32;

	if (lba48_sectors == 0)
		kSATASectorCount = (identify_data[61] << 16) | identify_data[60];
	else
		kSATASectorCount = lba48_sectors;

	/// Show what we got.
	
	kout << "Disk Model: " << kCurrentDiskModel << kendl;
	kout << "Disk Size: " << number(drv_get_size()) << kendl;
	kout << "Disk Sector Count: " << number(kSATASectorCount) << kendl;
}

/// @brief Finds a command slot for a HBA port.
/// @param port The port to search on.
/// @return The slot, or ~0.
STATIC Int32 drv_find_cmd_slot_ahci(HbaPort* port) noexcept
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

/// @brief Send an AHCI command, according to the template parameters.
/// @param lba Logical Block Address to look for.
/// @param buffer The data buffer to transfer.
/// @param sector_sz The disk's sector size (unused)
/// @param size_buffer The size of the **buffer** parameter.
template <BOOL Write, BOOL CommandOrCTRL, BOOL Identify>
STATIC Void drv_std_input_output_ahci(UInt64 lba, UInt8* buffer, SizeT sector_sz, SizeT size_buffer) noexcept
{
	UIntPtr slot = 0UL;

	slot = drv_find_cmd_slot_ahci(&kSATAHba->Ports[kSATAIndex]);

	if (slot == ~0)
	{
		err_global_get() = kErrorDisk;
		return;
	}

	if (!Write)
	{
		// Zero-memory the buffer field.
		rt_set_memory(buffer, 0, size_buffer);
	}

	/// prepare command header.
	volatile HbaCmdHeader* command_header = ((volatile HbaCmdHeader*)(((UInt64)kSATAHba->Ports[kSATAIndex].Clb)));

	/// Offset to specific command slot.
	command_header += slot;

	MUST_PASS(command_header);

	constexpr const UInt32 kMaxPRDSize = mib_cast(4);

	command_header->Cfl	  = sizeof(FisRegH2D) / sizeof(UInt32);
	command_header->Write = Write;
	command_header->Prdtl = (UInt16)((size_buffer + kMaxPRDSize - 1) / kMaxPRDSize);

	volatile HbaCmdTbl* command_table = (volatile HbaCmdTbl*)((VoidPtr)((UInt64)command_header->Ctba));

	rt_set_memory((HbaCmdTbl*)command_table, 0, sizeof(HbaCmdTbl) + (command_header->Prdtl - 1) * sizeof(HbaPrdtEntry));

	MUST_PASS(command_table);

	UIntPtr buffer_phys		= HAL::hal_get_phys_address(buffer);
	SizeT	bytes_remaining = size_buffer;
	SizeT	prdt_count		= command_header->Prdtl;

	for (UInt16 i = 0; i < prdt_count; ++i)
	{
		UInt32 chunk = (bytes_remaining > kMaxPRDSize) ? kMaxPRDSize : bytes_remaining;

		command_table->Prdt[i].Dba	= (UInt32)(buffer_phys & 0xFFFFFFFF);
		command_table->Prdt[i].Dbau = (UInt32)(buffer_phys >> 32);
		command_table->Prdt[i].Dbc	= chunk - 1;
		command_table->Prdt[i].Ie	= 1;

		buffer_phys += chunk;
		bytes_remaining -= chunk;
	}

	volatile FisRegH2D* h2d_fis = (volatile FisRegH2D*)(&command_table->Cfis);

	rt_set_memory((FisRegH2D*)h2d_fis, 0, sizeof(FisRegH2D));

	h2d_fis->FisType   = kFISTypeRegH2D;
	h2d_fis->CmdOrCtrl = CommandOrCTRL;
	h2d_fis->Command   = (Identify ? (kAHCICmdIdentify) : (Write ? kAHCICmdWriteDmaEx : kAHCICmdReadDmaEx));

	h2d_fis->Lba0 = (lba)&0xFF;
	h2d_fis->Lba1 = (lba >> 8) & 0xFF;
	h2d_fis->Lba2 = (lba >> 16) & 0xFF;

	h2d_fis->Device = kSATALBAMode;

	h2d_fis->Lba3 = (lba >> 24) & 0xFF;
	h2d_fis->Lba4 = (lba >> 32) & 0xFF;
	h2d_fis->Lba5 = (lba >> 40) & 0xFF;

	h2d_fis->CountLow  = (size_buffer)&0xFF;
	h2d_fis->CountHigh = (size_buffer >> 8) & 0xFF;

	while (kSATAHba->Ports[kSATAIndex].Tfd & (kSATASRBsy | kSATASRDrq))
	{
		;
	}

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

	/// we should wait again, just in case.
	while (kSATAHba->Ports[kSATAIndex].Tfd & (kSATASRBsy | kSATASRDrq))
	{
		;
	}

	if (kSATAHba->Is & kHBAErrTaskFile)
	{
		err_global_get() = kErrorDiskIsCorrupted;
		return;
	}

	err_global_get() = kErrorSuccess;
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

/// @brief Enable Host and probe using the IDENTIFY command.
STATIC Void ahci_enable_and_probe()
{
	kSATAHba->Ports[kSATAIndex].Cmd &= ~kHBAPxCmdFre;
	kSATAHba->Ports[kSATAIndex].Cmd &= ~kHBAPxCmdST;

	while (YES)
	{
		if (kSATAHba->Ports[kSATAIndex].Cmd & kHBAPxCmdCR)
			continue;

		if (kSATAHba->Ports[kSATAIndex].Cmd & kHBAPxCmdFR)
			continue;

		break;
	}

	// Command engine stopped, remap the AHCI port.

	auto port = &kSATAHba->Ports[kSATAIndex];

	// Relocate Command List Base.

	constexpr UIntPtr const kAHCIBaseAddress = mib_cast(4);

	port->Clb  = kAHCIBaseAddress + (kSATAIndex << 10);
	port->Clbu = 0;

	// clean it.
	rt_set_memory(reinterpret_cast<VoidPtr>(port->Clb), 0, 1024);

	// Relocate Frame Info Structure now.

	port->Fb  = kAHCIBaseAddress + (kSATAPortCnt << 10) + (kSATAIndex << 10);
	port->Fbu = 0;

	// clean it.
	rt_set_memory(reinterpret_cast<VoidPtr>(port->Fb), 0, 256);

	volatile HbaCmdHeader* cmd_hdr = reinterpret_cast<volatile HbaCmdHeader*>(port->Clb);

	for (Int32 i = 0; i < kSATAPortCnt; i++)
	{
		cmd_hdr[i].Prdtl = 8;
		cmd_hdr[i].Ctba	 = kAHCIBaseAddress + (40 << 10) + (kSATAPortCnt << 10) + (kSATAIndex << 10);
		cmd_hdr[i].Ctbau = 0;

		rt_set_memory(reinterpret_cast<VoidPtr>(cmd_hdr[i].Ctba), 0, 256);
	}

	// Now we are ready.

	kSATAHba->Ports[kSATAIndex].Cmd |= kHBAPxCmdFre;
	kSATAHba->Ports[kSATAIndex].Cmd |= kHBAPxCmdST;

	if (kSATAHba->Bohc & kHBABohcBiosOwned)
	{
		kSATAHba->Bohc |= kHBABohcOSOwned;

		while (kSATAHba->Bohc & kHBABohcBiosOwned)
		{
			;
		}
	}

	drv_compute_disk_ahci();
}

/// @brief Initializes an AHCI disk.
/// @param pi the amount of ports that have been detected.
/// @param atapi reference value, tells whether we should detect ATAPI instead of SATA.
/// @return if the disk was successfully initialized or not.
STATIC Bool drv_std_init_ahci(UInt16& pi, BOOL& atapi)
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

			pi = ports_implemented;

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
					kSATAIndex = ahci_index;
					kSATAHba   = mem_ahci;

					ahci_enable_and_probe();

					break;
				}
				else if (atapi && kSATAPISignature == mem_ahci->Ports[ahci_index].Sig)
				{
					kSATAIndex = ahci_index;
					kSATAHba   = mem_ahci;

					ahci_enable_and_probe();

					break;
				}

				ports_implemented >>= 1;
				++ahci_index;
			}

			err_global_get() = kErrorSuccess;

			return YES;
		}
	}

	err_global_get() = kErrorDisk;

	return NO;
}

/// @brief Checks if an AHCI device is detected.
/// @return Either if detected, or not found.
Bool drv_std_detected_ahci()
{
	return kSATADev.DeviceId() != (UShort)PCI::PciConfigKind::Invalid && kSATADev.Bar(kSATABar5) != 0;
}

// ================================================================================================

//
//	This applies only if we compile with AHCI as a default disk driver.
//

// ================================================================================================

#ifdef __AHCI__

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Void drv_std_write(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer)
{
	drv_std_input_output_ahci<YES, YES, NO>(lba, reinterpret_cast<UInt8*>(buffer), sector_sz, size_buffer);
}

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Void drv_std_read(UInt64 lba, Char* buffer, SizeT sector_sz, SizeT size_buffer)
{
	drv_std_input_output_ahci<NO, YES, NO>(lba, reinterpret_cast<UInt8*>(buffer), sector_sz, size_buffer);
}

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Bool drv_std_init(UInt16& pi)
{
	BOOL atapi = NO;
	return drv_std_init_ahci(pi, atapi);
}

////////////////////////////////////////////////////
///
////////////////////////////////////////////////////
Bool drv_std_detected(Void)
{
	return drv_std_detected_ahci();
}

////////////////////////////////////////////////////
/**
	@brief Gets the number of sectors inside the drive.
	@return Sector size in bytes.
 */
////////////////////////////////////////////////////
SizeT drv_get_sector_count()
{
	return drv_get_sector_count_ahci();
}

////////////////////////////////////////////////////
/// @brief Get the drive size.
/// @return Disk size in bytes.
////////////////////////////////////////////////////
SizeT drv_get_size()
{
	return drv_get_size_ahci();
}

#endif // ifdef __AHCI__

namespace Kernel
{
	/// @brief Initialize an AHCI device (StorageKit)
	UInt16 sk_init_ahci_device(BOOL atapi)
	{
		UInt16 pi = 0;
		return drv_std_init_ahci(pi, atapi);

		kSATAPortsImplemented = pi;

		return pi;
	}

	/// @brief Implementation details namespace.
	namespace Detail
	{
		/// @brief Read AHCI device.
		/// @param self device
		/// @param mnt mounted disk.
		STATIC Void sk_io_read_ahci(IDeviceObject<MountpointInterface*>* self, MountpointInterface* mnt)
		{
			AHCIDeviceInterface* dev = (AHCIDeviceInterface*)self;

			err_global_get() = kErrorDisk;

			if (!dev)
				return;

			auto disk = mnt->GetAddressOf(dev->GetIndex());

			if (!disk)
				return;

			err_global_get() = kErrorSuccess;

			drv_std_input_output_ahci<NO, YES, NO>(disk->fPacket.fPacketLba, (UInt8*)disk->fPacket.fPacketContent, kAHCISectorSize, disk->fPacket.fPacketSize);
		}

		/// @brief Write AHCI device.
		/// @param self device
		/// @param mnt mounted disk.
		STATIC Void sk_io_write_ahci(IDeviceObject<MountpointInterface*>* self, MountpointInterface* mnt)
		{
			AHCIDeviceInterface* dev = (AHCIDeviceInterface*)self;

			err_global_get() = kErrorDisk;

			if (!dev)
				return;

			auto disk = mnt->GetAddressOf(dev->GetIndex());

			if (!disk)
				return;

			err_global_get() = kErrorSuccess;

			drv_std_input_output_ahci<YES, YES, NO>(disk->fPacket.fPacketLba, (UInt8*)disk->fPacket.fPacketContent, kAHCISectorSize, disk->fPacket.fPacketSize);
		}
	} // namespace Detail

	/// @brief Acquires a new AHCI device with drv_index in mind.
	/// @param drv_index The drive index to assign.
	/// @return A wrapped device interface if successful, or error code.
	ErrorOr<AHCIDeviceInterface> sk_acquire_ahci_device(Int32 drv_index)
	{
		if (!drv_std_detected_ahci())
			return ErrorOr<AHCIDeviceInterface>(kErrorDisk);

		AHCIDeviceInterface device(Detail::sk_io_read_ahci,
								   Detail::sk_io_write_ahci,
								   nullptr);

		device.SetPortsImplemented(kSATAPortsImplemented);
		device.SetIndex(drv_index);

		return ErrorOr<AHCIDeviceInterface>(device);
	}
} // namespace Kernel
