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
/// @brief Drive Manager API.
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
		drv_std_read(pckt->fLba, (Char*)pckt->fPacketContent.Leak(), kAHCISectorSize, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_read(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent.Leak(), kATASectorSize, pckt->fPacketSize);
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
		drv_std_write(pckt->fLba, (Char*)pckt->fPacketContent.Leak(), kAHCISectorSize, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_write(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent.Leak(), kATASectorSize, pckt->fPacketSize);
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

		pckt->fPacketGood = false;

#if defined(__ATA_PIO__) || defined(__ATA_DMA__)
		kATAMaster = true;
		kATAIO	   = ATA_PRIMARY_IO;

		if (!drv_std_init(kATAIO, kATAMaster, kATAIO, kATAMaster))
			return;

#elif defined(__AHCI__)
		UInt16 pi = 0;

		if (!drv_std_init(pi))
			return;
#endif // if defined(__ATA_PIO__) || defined (__ATA_DMA__)

		pckt->fPacketGood = true;
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
	DriveTrait io_construct_drive() noexcept
	{
		DriveTrait trait;

		rt_copy_memory((VoidPtr) "/Mount/NUL:", trait.fName, rt_string_len("/Mount/NUL:"));
		trait.fKind = kInvalidStorage;

		trait.fInput	 = io_drv_unimplemented;
		trait.fOutput	 = io_drv_unimplemented;
		trait.fVerify	 = io_drv_unimplemented;
		trait.fInit		 = io_drv_unimplemented;
		trait.fDriveKind = io_drv_kind;

		kcout << "Construct: " << trait.fName << ".\r";

		return trait;
	}

	namespace Detail
	{
		Void ioi_detect_drive(DriveTrait& trait)
		{
			_BOOT_BLOCK_STRUCT block_struct;

			trait.fPacket.fLba			 = kEPMBaseLba;
			trait.fPacket.fPacketSize	 = sizeof(_BOOT_BLOCK_STRUCT);
			trait.fPacket.fPacketContent = &block_struct;

			io_drv_input(&trait.fPacket);

			trait.fKind = kMassStorage;

			if (rt_string_cmp(block_struct.Magic, kEPMMagic, kEPMMagicLength) == 0)
			{
				trait.fKind |= kEPMDrive;
				kcout << "Formatted drive is EPM.\r";
			}
			else
			{
				trait.fKind |= kUnformattedDrive;
				kcout << "Formatted drive is blank.\r";
			}

			trait.fPacket.fLba			 = 0;
			trait.fPacket.fPacketSize	 = 0UL;
			trait.fPacket.fPacketContent = nullptr;
		}
	} // namespace Detail

	/// @brief Fetches the main drive.
	/// @return the new drive. (returns kEPMDrive if EPM formatted)
	DriveTrait io_construct_main_drive() noexcept
	{
		DriveTrait trait;

		rt_copy_memory((VoidPtr) "/Mount/OS:", trait.fName, rt_string_len("/Mount/OS:"));

		trait.fVerify	 = io_drv_unimplemented;
		trait.fOutput	 = io_drv_output;
		trait.fInput	 = io_drv_input;
		trait.fInit		 = io_drv_init;
		trait.fDriveKind = io_drv_kind;

		Detail::ioi_detect_drive(trait);

		kcout << "Construct: " << trait.fName << ".\r";

		return trait;
	}
} // namespace Kernel
