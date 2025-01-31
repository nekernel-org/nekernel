/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef INC_DRIVE_MANAGER_H
#define INC_DRIVE_MANAGER_H

#include <KernelKit/UserProcessScheduler.h>
#include <CompilerKit/CompilerKit.h>
#include <KernelKit/DebugOutput.h>
#include <KernelKit/DeviceMgr.h>
#include <KernelKit/LPC.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <NewKit/Ref.h>

#define kDriveMaxCount	(4U)
#define kDriveSectorSz	(512U)
#define kDriveInvalidID (-1)
#define kDriveNameLen	(32)

#define drv_sector_cnt(SIZE, SECTOR_SZ) (((SIZE) + (SECTOR_SZ)) / (SECTOR_SZ))

namespace Kernel
{
	enum
	{
		kInvalidDisc = -1,

		/// Storage types, combine with flags.
		kBlockDevice	 = 0xAD,
		kMassStorageDisc = 0xDA,
		kFloppyDisc		 = 0xCD,
		kOpticalDisc	 = 0xDC, // CD-ROM/DVD-ROM/Blu-Ray
		kTapeDisc		 = 0xD7,

		/// Storage flags, combine with types.
		kReadOnlyDrive	  = 0x10, // Read only drive
		kEPMDrive		  = 0x11, // Explicit Partition Map.
		kEPTDrive		  = 0x12, // ESP w/ EPM partition.
		kMBRDrive		  = 0x13, // PC classic partition scheme
		kGPTDrive		  = 0x14, // PC new partition scheme
		kUnformattedDrive = 0x15,
		kStorageCount	  = 9,
	};

	/// @brief Media drive trait type.
	struct DriveTrait final
	{
		Char  fName[kDriveNameLen]; // /System, /Boot, //./Devices/USB...
		Int32 fKind;				// fMassStorage, fFloppy, fOpticalDisc.
		Int32 fFlags;				// fReadOnly, fEPMDrive...

		/// @brief Packet drive (StorageKit compilant.)
		struct DrivePacket final
		{
			VoidPtr fPacketContent{nullptr};			//! packet body.
			Char	fPacketMime[kDriveNameLen] = "*/*"; //! identify what we're sending.
			SizeT	fPacketSize{0UL};					//! packet size
			UInt32	fPacketCRC32{0UL};					//! sanity crc, in case if good is set to false
			Boolean fPacketGood{YES};
			Lba		fPacketLba{0UL};
			Boolean fPacketReadOnly{NO};
		} fPacket;

		Lba	  fLbaStart{0}, fLbaEnd{0};
		SizeT fSectorSz{512};

		Void (*fInput)(DrivePacket packet);
		Void (*fOutput)(DrivePacket packet);
		Void (*fVerify)(DrivePacket packet);
		Void (*fInit)(DrivePacket packet);
		const Char* (*fDriveKind)(Void);
	};

	///! drive as a device.
	typedef DriveTrait* DriveTraitPtr;

	/**
	 * @brief Mounted drives interface.
	 * @note This class has all of it's drive set to nullptr, allocate them using
	 * GetAddressOf(index).
	 */
	class MountpointInterface final
	{
	public:
		explicit MountpointInterface() = default;
		~MountpointInterface()		   = default;

		NE_COPY_DEFAULT(MountpointInterface);

	public:
		DriveTrait& A()
		{
			return mA;
		}

		DriveTrait& B()
		{
			return mB;
		}

		DriveTrait& C()
		{
			return mC;
		}

		DriveTrait& D()
		{
			return mD;
		}

		enum
		{
			kDriveIndexA = 0,
			kDriveIndexB,
			kDriveIndexC,
			kDriveIndexD,
			kDriveIndexInvalid,
		};

		DriveTraitPtr GetAddressOf(const Int32& index)
		{
			err_local_get() = kErrorSuccess;

			switch (index)
			{
			case kDriveIndexA:
				return &mA;
			case kDriveIndexB:
				return &mB;
			case kDriveIndexC:
				return &mC;
			case kDriveIndexD:
				return &mD;
			default: {
				err_local_get() = kErrorNoSuchDisk;
				kcout << "No such disc letter.\n";

				break;
			}
			}

			return nullptr;
		}

	private:
		DriveTrait mA, mB, mC, mD;
	};

	/// @brief Unimplemented drive.
	/// @param pckt the packet to read.
	/// @return
	Void io_drv_unimplemented(DriveTrait::DrivePacket* pckt) noexcept;

	/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
	/// @param void none.
	/// @return the drive kind (ATA, Flash, NVM)
	const Char* io_drv_kind(Void);

	/// @brief Makes a new drive.
	/// @return the new drive as a trait.
	DriveTrait io_construct_blank_drive(Void) noexcept;

	/// @brief Fetches the main drive.
	/// @return the new drive as a trait.
	DriveTrait io_construct_main_drive(Void) noexcept;

	namespace Detect
	{
		Void io_detect_drive(DriveTrait& trait);
	}

	/// @brief Read from newfs disk.
	/// @param Mnt mounted interface.
	/// @param DrvTrait drive info
	/// @param DrvIndex drive index.
	/// @return
	Int32 fs_ifs_read(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex);

	/// @brief Write to ifs disk.
	/// @param Mnt mounted interface.
	/// @param DrvTrait drive info
	/// @param DrvIndex drive index.
	/// @return
	Int32 fs_ifs_write(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex);
} // namespace Kernel

#endif /* ifndef INC_DRIVE_MANAGER_H */
