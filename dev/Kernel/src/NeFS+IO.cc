/* -------------------------------------------

	Copyright (C) 2024, t& Corporation, all rights reserved.

------------------------------------------- */

#include <KernelKit/DriveMgr.h>
#include <KernelKit/FileMgr.h>

/*************************************************************
 *
 * File: NeFS+IO.cc
 * Purpose: Filesystem to mountpoint interface.
 * Date: 3/26/24
 *
 * Copyright (C) 2024, t& Corporation, all rights reserved., all rights reserved.
 *
 *************************************************************/

#ifdef __FSKIT_INCLUDES_NEFS__

#include <FirmwareKit/EPM.h>

/// Useful macros.

#define rtl_nefs_write(DRV, TRAITS, MP) (MP->DRV()).fOutput(&TRAITS)
#define rtl_nefs_read(DRV, TRAITS, MP)	(MP->DRV()).fInput(&TRAITS)

using namespace Kernel;

/// @brief Read from newfs disk.
/// @param Mnt mounted interface.
/// @param DrvTrait drive info
/// @param DrvIndex drive index.
/// @return
Int32 fs_nefs_read(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex)
{
	if (!Mnt)
		return 1;

	DrvTrait.fPacket.fPacketGood = false;

	switch (DrvIndex)
	{
	case kNeFSSubDriveA: {
		rtl_nefs_read(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveB: {
		rtl_nefs_read(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveC: {
		rtl_nefs_read(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveD: {
		rtl_nefs_read(D, DrvTrait.fPacket, Mnt);
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
Int32 fs_nefs_write(MountpointInterface* Mnt, DriveTrait& DrvTrait, Int32 DrvIndex)
{
	if (!Mnt)
		return 1;

	DrvTrait.fPacket.fPacketGood = false;

	switch (DrvIndex)
	{
	case kNeFSSubDriveA: {
		rtl_nefs_write(A, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveB: {
		rtl_nefs_write(B, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveC: {
		rtl_nefs_write(C, DrvTrait.fPacket, Mnt);
		break;
	}
	case kNeFSSubDriveD: {
		rtl_nefs_write(D, DrvTrait.fPacket, Mnt);
		break;
	}
	}

	return DrvTrait.fPacket.fPacketGood;
}

#endif // ifdef __FSKIT_INCLUDES_NEFS__
