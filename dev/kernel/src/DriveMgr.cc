/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/DriveMgr.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <modules/ATA/ATA.h>
#include <modules/AHCI/AHCI.h>
#include <modules/NVME/NVME.h>

/***********************************************************************************/
/// @file DriveMgr.cc
/// @brief Drive Manager of kernel.
/***********************************************************************************/

namespace Kernel
{
#if defined(__ATA_PIO__) || defined(__ATA_DMA__)
	STATIC UInt16 kATAIO	 = 0U;
	STATIC UInt8  kATAMaster = 0U;
#endif

#if defined(__AHCI__)
	STATIC UInt16 kAHCIPortsImplemented = 0UL;
#endif

	/// @brief reads from an ATA drive.
	/// @param pckt Packet structure (fPacketContent must be non null)
	/// @return
	Void io_drv_input(DriveTrait::DrivePacket pckt)
	{
#ifdef __AHCI__
		drv_std_read(pckt.fPacketLba, (Char*)pckt.fPacketContent, kAHCISectorSize, pckt.fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_read(pckt.fPacketLba, kATAIO, kATAMaster, (Char*)pckt.fPacketContent, kATASectorSize, pckt.fPacketSize);
#endif
	}

	/// @brief Writes to an ATA drive.
	/// @param pckt the packet to write.
	/// @return
	Void io_drv_output(DriveTrait::DrivePacket pckt)
	{
		if (pckt.fPacketReadOnly)
		{
			pckt.fPacketGood = NO;
			return;
		}

#ifdef __AHCI__
		drv_std_write(pckt.fPacketLba, (Char*)pckt.fPacketContent, kAHCISectorSize, pckt.fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_write(pckt.fPacketLba, kATAIO, kATAMaster, (Char*)pckt.fPacketContent, kATASectorSize, pckt.fPacketSize);
#endif
	}

	/// @brief Executes a disk check on the ATA drive.
	/// @param pckt the packet to read.
	/// @return
	Void io_drv_init(DriveTrait::DrivePacket pckt)
	{
#if defined(__ATA_PIO__) || defined(__ATA_DMA__)
		kATAMaster = 0;
		kATAIO	   = 0;

		kATAMaster = YES;
		kATAIO	   = ATA_PRIMARY_IO;

		if (drv_std_init(kATAIO, kATAMaster, kATAIO, kATAMaster))
		{
			pckt.fPacketGood = YES;
			return;
		}

		kATAMaster = NO;
		kATAIO	   = ATA_SECONDARY_IO;

		if (!drv_std_init(kATAIO, kATAMaster, kATAIO, kATAMaster))
		{
			pckt.fPacketGood = YES;
			return;
		}

		pckt.fPacketGood = YES;
#elif defined(__AHCI__)
		kAHCIPortsImplemented = 0;

		if (drv_std_init(kAHCIPortsImplemented))
		{
			pckt.fPacketGood = YES;
		}

#endif // if defined(__ATA_PIO__) || defined (__ATA_DMA__)
	}

/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
/// @param void no arguments.
/// @return no arguments.
#ifdef __ATA_PIO__
	const Char* io_drv_kind(Void)
	{
		return "ATA-PIO";
	}
#elif defined(__ATA_DMA__)
	const Char* io_drv_kind(Void)
	{
		return "ATA-DMA";
	}
#elif defined(__AHCI__)
	const Char* io_drv_kind(Void)
	{
		return "AHCI";
	}
#else
	const Char* io_drv_kind(Void)
	{
		return "null";
	}
#endif

	/// @brief Unimplemented drive function.
	/// @param pckt the packet to read.
	Void io_drv_unimplemented(DriveTrait::DrivePacket pckt) noexcept
	{
		NE_UNUSED(pckt);
	}

	/// @brief Makes a new drive.
	/// @return the new blank drive.
	DriveTrait io_construct_blank_drive() noexcept
	{
		DriveTrait trait;

		constexpr auto kBlankDrive = "/media/blank/";

		rt_copy_memory((VoidPtr)kBlankDrive, trait.fName, rt_string_len(kBlankDrive));
		trait.fKind = kInvalidDrive;

		trait.fInput	= io_drv_unimplemented;
		trait.fOutput	= io_drv_unimplemented;
		trait.fVerify	= io_drv_unimplemented;
		trait.fInit		= io_drv_unimplemented;
		trait.fProtocol = io_drv_kind;

		kout << "Construct: " << trait.fName << "\r";

		return trait;
	}

	namespace Detail
	{
		Void io_detect_drive(DriveTrait& trait)
		{
			trait.fInit(trait.fPacket);

			EPM_PART_BLOCK block_struct;

			trait.fPacket.fPacketLba	 = kEPMBootBlockLba;
			trait.fPacket.fPacketSize	 = sizeof(EPM_PART_BLOCK);
			trait.fPacket.fPacketContent = &block_struct;

			rt_copy_memory((VoidPtr) "fs/detect-packet", trait.fPacket.fPacketMime,
						   rt_string_len("fs/detect-packet"));

			trait.fInput(trait.fPacket);

			if (rt_string_cmp(((EPM_PART_BLOCK*)trait.fPacket.fPacketContent)->Magic, kEPMMagic, kEPMMagicLength) == 0)
			{
				trait.fPacket.fPacketReadOnly = NO;
				trait.fKind					  = kMassStorageDrive | kEPMDrive;

				kout << "Disk is EPM.\r";

				trait.fSectorSz = block_struct.SectorSz;
				trait.fLbaEnd	= block_struct.LbaEnd;
				trait.fLbaStart = block_struct.LbaStart;
			}
			else
			{
				trait.fPacket.fPacketReadOnly = YES;
				trait.fKind					  = kMassStorageDrive | kUnformattedDrive | kReadOnlyDrive;

				if (block_struct.Name[0] == 0 ||
					!rt_is_alnum(block_struct.Name[0]))
				{
					kout << "Disk partition is empty (Read Only)\r";
				}
				else
				{
					kout << "Scheme Found: " << block_struct.Name << kendl;
				}
			}

			rt_copy_memory((VoidPtr) "*/*", trait.fPacket.fPacketMime,
						   rt_string_len("*/*"));

			trait.fPacket.fPacketLba	 = 0;
			trait.fPacket.fPacketSize	 = 0UL;
			trait.fPacket.fPacketContent = nullptr;
		}
	} // namespace Detail

	/// @brief Fetches the main drive.
	/// @return the new drive. (returns kEPMDrive if EPM formatted)
	DriveTrait io_construct_main_drive() noexcept
	{
		DriveTrait trait;

		constexpr auto kMainDrive = "/media/sda/";

		rt_copy_memory((VoidPtr)kMainDrive, trait.fName, rt_string_len(kMainDrive));

		MUST_PASS(trait.fName[0] != 0);

		trait.fVerify	= io_drv_unimplemented;
		trait.fOutput	= io_drv_output;
		trait.fInput	= io_drv_input;
		trait.fInit		= io_drv_init;
		trait.fProtocol = io_drv_kind;

		kout << "Detecting partition scheme of: " << trait.fName << ".\r";

		Detail::io_detect_drive(trait);

		return trait;
	}
} // namespace Kernel
