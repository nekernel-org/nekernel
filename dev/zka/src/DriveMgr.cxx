/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/DebugOutput.hxx>
#include <KernelKit/DriveMgr.hxx>
#include <NewKit/Utils.hxx>

#include <modules/ATA/ATA.hxx>
#include <modules/AHCI/AHCI.hxx>
#include <modules/NVME/NVME.hxx>

/// @file DriveMgr.cxx
/// @brief Drive manager.

namespace Kernel
{
	STATIC UInt16 kATAIO	 = 0U;
	STATIC UInt8  kATAMaster = 0U;

	/// @brief reads from an ATA drive.
	/// @param pckt
	/// @return
	Void io_drv_input(DriveTrait::DrivePacket* pckt)
	{
		if (!pckt)
		{
			return;
		}

#ifdef __AHCI__
		drv_std_read(pckt->fLba, (Char*)pckt->fPacketContent, kAHCISectorSize, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_read(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, kATASectorSize, pckt->fPacketSize);
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
		drv_std_write(pckt->fLba, (Char*)pckt->fPacketContent, kAHCISectorSize, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
		drv_std_write(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, kATASectorSize, pckt->fPacketSize);
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

		MUST_PASS(drv_std_init(kATAIO, kATAMaster, kATAIO, kATAMaster));
#elif defined(__AHCI__)
		UInt16 pi = 0;
		MUST_PASS(drv_std_init(pi));
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

	/// @brief Unimplemented drive.
	/// @param pckt
	/// @return
	Void io_drv_unimplemented(DriveTrait::DrivePacket* pckt)
	{
		ZKA_UNUSED(pckt);
	}

	/// @brief Makes a new drive.
	/// @return the new drive.
	DriveTrait io_construct_drive() noexcept
	{
		DriveTrait trait{};

		rt_copy_memory((VoidPtr) "\\Mount\\NUL:", trait.fName, rt_string_len("\\Mount\\NUL:"));
		trait.fKind = kInvalidDrive;

		trait.fInput	 = io_drv_unimplemented;
		trait.fOutput	 = io_drv_unimplemented;
		trait.fVerify	 = io_drv_unimplemented;
		trait.fInit		 = io_drv_unimplemented;
		trait.fDriveKind = io_drv_kind;

		return trait;
	}

	/// @brief Fetches the main drive.
	/// @return the new drive.
	DriveTrait io_construct_main_drive() noexcept
	{
		DriveTrait trait{};

		rt_copy_memory((VoidPtr) "\\Mount\\MainDisk:", trait.fName, rt_string_len("\\Mount\\MainDisk:"));
		trait.fKind = kMassStorage;

		trait.fVerify	 = io_drv_unimplemented;
		trait.fOutput	 = io_drv_output;
		trait.fInput	 = io_drv_input;
		trait.fInit		 = io_drv_init;
		trait.fDriveKind = io_drv_kind;

		kcout << "Constructed drive successfully..\r";

		return trait;
	}
} // namespace Kernel
