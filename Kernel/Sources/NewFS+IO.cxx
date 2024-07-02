/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/DriveManager.hxx>
#include <KernelKit/FileManager.hpp>

/*************************************************************
 *
 * File: NewFS+IO.cxx
 * Purpose: Filesystem to mountpoint interface.
 * Date: 3/26/24
 *
 * Copyright Zeta Electronics Corporation, all rights reserved.
 *
 *************************************************************/

#ifdef __FSKIT_NEWFS__

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
	case kNewFSSubDriveA: {
		NEWFS_READ(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNewFSSubDriveB: {
		NEWFS_READ(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNewFSSubDriveC: {
		NEWFS_READ(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNewFSSubDriveD: {
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
	case kNewFSSubDriveA: {
		NEWFS_WRITE(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNewFSSubDriveB: {
		NEWFS_WRITE(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNewFSSubDriveC: {
		NEWFS_WRITE(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNewFSSubDriveD: {
		NEWFS_WRITE(D, DrvTrait.fPacket, Mnt);
		break;
	}
	}

	return DrvTrait.fPacket.fPacketGood;
}

#endif // ifdef __FSKIT_NEWFS__
