/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <KernelKit/DriveMgr.h>
#include <KernelKit/FileMgr.h>

/*************************************************************
 *
 * File: NeFS+IO.cxx
 * Purpose: Filesystem to mountpoint interface.
 * Date: 3/26/24
 *
 * Copyright ZKA Web Services Co., all rights reserved.
 *
 *************************************************************/

#ifdef __FSKIT_INCLUDES_NEFS__

#include <FirmwareKit/EPM.h>

/// Useful macros.

#define NEFS_WRITE(DRV, TRAITS, MP) (MP->DRV()).fOutput(&TRAITS)
#define NEFS_READ(DRV, TRAITS, MP)	(MP->DRV()).fInput(&TRAITS)

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
		NEFS_READ(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveB: {
		NEFS_READ(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveC: {
		NEFS_READ(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveD: {
		NEFS_READ(D, DrvTrait.fPacket, Mnt);
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
		NEFS_WRITE(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveB: {
		NEFS_WRITE(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveC: {
		NEFS_WRITE(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveD: {
		NEFS_WRITE(D, DrvTrait.fPacket, Mnt);
		break;
	}
	}

	return DrvTrait.fPacket.fPacketGood;
}

#endif // ifdef __FSKIT_INCLUDES_NEFS__
