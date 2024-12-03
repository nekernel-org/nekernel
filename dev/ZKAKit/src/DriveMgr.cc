/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <Modules/ATA/ATA.h>
#include <Modules/AHCI/AHCI.h>
#include <Modules/NVME/NVME.h>

/***********************************************************************************/
/// @file DriveMgr.cc
/// @brief Drive Manager of minoskrnl.
/***********************************************************************************/

namespace Kernel
{
	STATIC UInt16 kATAIO	 = 0U;
	STATIC UInt8  kATAMaster = 0U;

	/// @brief reads from an ATA drive.
	/// @param pckt Packet structure (fPacketContent must be non null)
	/// @return
	Void io_drv_input(DriveTrait::DrivePacket* pckt)
	{
		if (!pckt)
		{
			return;
		}

#ifdef __AHCI__
		drv_std_read(pckt->fPacketLba, (Char*)pckt->fPacketContent, kAHCISectorSize, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_read(pckt->fPacketLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, kATASectorSize, pckt->fPacketSize);
#endif
	}

	/// @brief Writes to an ATA drive.
	/// @param pckt the packet to write.
	/// @return
	Void io_drv_output(DriveTrait::DrivePacket* pckt)
	{
		if (!pckt)
		{
			return;
		}

#ifdef __AHCI__
		drv_std_write(pckt->fPacketLba, (Char*)pckt->fPacketContent, kAHCISectorSize, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_write(pckt->fPacketLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, kATASectorSize, pckt->fPacketSize);
#endif
	}

	/// @brief Executes a disk check on the ATA drive.
	/// @param pckt the packet to read.
	/// @return
	Void io_drv_init(DriveTrait::DrivePacket* pckt)
	{
		if (!pckt)
		{
			return;
		}

		kATAMaster = 0;
		kATAIO	   = 0;

#if defined(__ATA_PIO__) || defined(__ATA_DMA__)
		kATAMaster = true;
		kATAIO	   = ATA_PRIMARY_IO;

		if (drv_std_init(kATAIO, kATAMaster, kATAIO, kATAMaster))
		{
			pckt->fPacketGood = YES;
			return;
		}

		kATAMaster = false;
		kATAIO	   = ATA_SECONDARY_IO;

		if (!drv_std_init(kATAIO, kATAMaster, kATAIO, kATAMaster))
		{
			return;
		}

		pckt->fPacketGood = YES;
#elif defined(__AHCI__)
		UInt16 pi = 0;

		if (!drv_std_init(pi))
		{
			return;
		}
#endif // if defined(__ATA_PIO__) || defined (__ATA_DMA__)
	}

/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
/// @param no arguments.
/// @return no arguments.
#ifdef __ATA_PIO__
	const Char* io_drv_kind(Void)
	{
		return "ATA-PIO";
	}
#endif
#ifdef __ATA_DMA__
	const Char* io_drv_kind(Void)
	{
		return "ATA-DMA";
	}
#endif
#ifdef __AHCI__
	const Char* io_drv_kind(Void)
	{
		return "AHCI";
	}
#endif
#ifdef __ZKA_MINIMAL_OS__
	const Char* io_drv_kind(Void)
	{
		return "Not Loaded";
	}
#endif

	/// @brief Unimplemented drive function.
	/// @param pckt the packet to read.
	/// @return
	Void io_drv_unimplemented(DriveTrait::DrivePacket* pckt)
	{
		ZKA_UNUSED(pckt);
	}

	/// @brief Makes a new drive.
	/// @return the new blank drive.
	DriveTrait io_construct_blank_drive() noexcept
	{
		DriveTrait trait;

		rt_copy_memory((VoidPtr) "/Disks/NUL:", trait.fName, rt_string_len("/Disks/NUL:"));
		trait.fKind = kInvalidDisc;

		trait.fInput	 = io_drv_unimplemented;
		trait.fOutput	 = io_drv_unimplemented;
		trait.fVerify	 = io_drv_unimplemented;
		trait.fInit		 = io_drv_unimplemented;
		trait.fDriveKind = io_drv_kind;

		kcout << "Construct: " << trait.fName << "\r";

		return trait;
	}

	namespace Detail
	{
		Void ioi_detect_drive(DriveTrait* trait)
		{
			static _BOOT_BLOCK_STRUCT block_struct;

			trait->fPacket.fPacketLba	  = kEPMBaseLba;
			trait->fPacket.fPacketSize	  = sizeof(_BOOT_BLOCK_STRUCT);
			trait->fPacket.fPacketContent = &block_struct;

			rt_copy_memory((VoidPtr) "fs/detect-packet", trait->fPacket.fPacketMime,
						   rt_string_len("fs/detect-packet"));

			trait->fInit(&trait->fPacket);

			trait->fInput(&trait->fPacket);

			if (rt_string_cmp(((BOOT_BLOCK_STRUCT*)trait->fPacket.fPacketContent)->Magic, kEPMMagic, kEPMMagicLength) == 0)
			{
				trait->fPacket.fPacketReadOnly = NO;
				trait->fKind				   = kMassStorageDisc | kEPMDrive;
				kcout << "Formatted Disc is EPM (Mass Storage).\r";
			}
			else
			{
				trait->fPacket.fPacketReadOnly = YES;
				trait->fKind				   = kMassStorageDisc | kUnformattedDrive | kReadOnlyDrive;
				kcout << "Scheme Found: " << block_struct.Name << endl;

				if (block_struct.Name[0] == 0)
					kcout << "Disc partition is unknown (set to Read Only).\r";
			}

			rt_copy_memory((VoidPtr) "*/*", trait->fPacket.fPacketMime,
						   rt_string_len("*/*"));

			trait->fPacket.fPacketLba	  = 0;
			trait->fPacket.fPacketSize	  = 0UL;
			trait->fPacket.fPacketContent = nullptr;
		}
	} // namespace Detail

	/// @brief Fetches the main drive.
	/// @return the new drive. (returns kEPMDrive if EPM formatted)
	DriveTrait io_construct_main_drive() noexcept
	{
		DriveTrait trait;

		rt_copy_memory((VoidPtr) "/Disks/OS:", trait.fName, rt_string_len("/Disks/OS:"));

		trait.fVerify	 = io_drv_unimplemented;
		trait.fOutput	 = io_drv_output;
		trait.fInput	 = io_drv_input;
		trait.fInit		 = io_drv_init;
		trait.fDriveKind = io_drv_kind;

		kcout << "Detect partiton scheme of: " << trait.fName << ".\r";

		Detail::ioi_detect_drive(&trait);

		return trait;
	}
} // namespace Kernel
