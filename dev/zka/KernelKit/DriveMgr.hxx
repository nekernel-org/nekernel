/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef __INC_DRIVE_MANAGER_HXX__
#define __INC_DRIVE_MANAGER_HXX__

#include <KernelKit/UserProcessScheduler.hxx>
#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <KernelKit/DeviceMgr.hxx>
#include <KernelKit/LPC.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/String.hxx>

#define kMaxDriveCountPerMountpoint (4U)

#define kDriveInvalidID (-1)
#define kDriveNameLen	(32)

#define DrvSectorCnt(SIZE, SECTOR_SZ) (((SIZE) + (SECTOR_SZ)) / (SECTOR_SZ))

namespace Kernel
{
	enum
	{
	    /// Storage type.
		kInvalidStorage = -1,
		kBlockDevice  = 0xAD,
		kMassStorage  = 0xDA,
		kFloppyDisc	  = 0xCD,
		kOpticalDisc  = 0xDC, // CD-ROM/DVD-ROM/Blu-Ray
		/// Storage flags, combine with below.
		kReadOnly = 0x10, // Read only drive
		kEPMDrive = 0x11, // Explicit Partition Map.
		kEPTDrive = 0x12, // ESP w/ EPM partition.
		kMBRDrive = 0x13, // PC classic partition scheme
		kGPTDrive = 0x14, // PC new partition scheme
		kStorageCount = 9,
	};

	typedef Int64 rt_drive_id_type;

	/// @brief Media drive trait type.
	struct DriveTrait final
	{
		Char			 fName[kDriveNameLen]; // /System, /Boot, //./Devices/USB...
		Int32			 fKind;				   // fMassStorage, fFloppy, fOpticalDisc.
		rt_drive_id_type fId;				   // Drive id.
		Int32			 fFlags;			   // fReadOnly, fXPMDrive, fXPTDrive

		/// @brief Packet drive (StorageKit compilant.)
		struct DrivePacket final
		{
			VoidPtr fPacketContent;				//! packet body.
			Char	fPacketMime[kDriveNameLen]; //! identify what we're sending.
			SizeT	fPacketSize;				//! packet size
			UInt32	fPacketCRC32;				//! sanity crc, in case if good is set to false
			Boolean fPacketGood;
			Lba		fLba;
		} fPacket;

		Void (*fInput)(DrivePacket* packetPtr);
		Void (*fOutput)(DrivePacket* packetPtr);
		Void (*fVerify)(DrivePacket* packetPtr);
		Void (*fInit)(DrivePacket* packetPtr);
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

		ZKA_COPY_DEFAULT(MountpointInterface);

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

		DriveTraitPtr GetAddressOf(Int32 index)
		{
			ErrLocal() = kErrorSuccess;

			switch (index)
			{
			case 0:
				return &mA;
			case 1:
				return &mB;
			case 2:
				return &mC;
			case 3:
				return &mD;
			default: {
				ErrLocal() = kErrorNoSuchDisk;
				kcout << "No such disk.\n";

				break;
			}
			}

			return nullptr;
		}

	private:
		DriveTrait mA, mB, mC, mD;
	};

	/// @brief Unimplemented drive.
	/// @param pckt
	/// @return
	Void io_drv_unimplemented(DriveTrait::DrivePacket* pckt);

	/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
	/// @param
	/// @return the drive kind (ATA, Flash, NVM)
	const Char* io_drv_kind(Void);

	/// @brief Makes a new drive.
	/// @return the new drive as a trait.
	DriveTrait io_construct_drive(void) noexcept;

	/// @brief Fetches the main drive.
	/// @return the new drive as a trait.
	DriveTrait io_construct_main_drive(void) noexcept;
} // namespace Kernel

#endif /* ifndef __INC_DRIVE_MANAGER_HXX__ */
