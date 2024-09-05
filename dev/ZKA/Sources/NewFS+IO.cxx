/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/DriveMgr.hxx>
#include <KernelKit/FileMgr.hxx>

/*************************************************************
 *
 * File: NeFS+IO.cxx
 * Purpose: Filesystem to mountpoint interface.
 * Date: 3/26/24
 *
 * Copyright ZKA Technologies., all rights reserved.
 *
 *************************************************************/

#ifdef __FSKIT_USE_NEWFS__

#include <FirmwareKit/EPM.hxx>

/// Useful macros.

#define NEWFS_WRITE(DRV, TRAITS, MP) (MP->DRV()).fOutput(&TRAITS)
#define NEWFS_READ(DRV, TRAITS, MP)	 (MP->DRV()).fInput(&TRAITS)

using namespace Kernel;

/// @brief Read from newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Int32 fs_newfs_read(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex)
{
	if (!Mnt)
		return -1;

	DrvTrait.fPacket.fPacketGood = false;

	switch (DrvIndex)
	{
	case kNeFSSubDriveA: {
		NEWFS_READ(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveB: {
		NEWFS_READ(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveC: {
		NEWFS_READ(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveD: {
		NEWFS_READ(D, DrvTrait.fPacket, Mnt);
		break;
	}
	}

	return DrvTrait.fPacket.fPacketGood;
}

/// @brief Write to newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Int32 fs_newfs_write(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex)
{
	if (!Mnt)
		return -1;

	DrvTrait.fPacket.fPacketGood = false;

	switch (DrvIndex)
	{
	case kNeFSSubDriveA: {
		NEWFS_WRITE(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveB: {
		NEWFS_WRITE(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveC: {
		NEWFS_WRITE(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveD: {
		NEWFS_WRITE(D, DrvTrait.fPacket, Mnt);
		break;
	}
	}

	return DrvTrait.fPacket.fPacketGood;
}

#endif // ifdef __FSKIT_USE_NEWFS__
