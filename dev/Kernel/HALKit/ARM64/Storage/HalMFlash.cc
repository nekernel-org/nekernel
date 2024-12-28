/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#ifdef ZKA_USE_MBCI_FLASH

#include <NewKit/Defines.h>
#include <ArchKit/ArchKit.h>
#include <Mod/MFlash/MFlash.h>

/// @file HalMFlash.cc
/// @brief MBCI Flash builtin.

#define kMaxFlash (4U)

namespace Kernel
{
	/// /Mount/Flash/n
	constexpr auto kFlashBridgeMagic	= "FLSH";
	constexpr auto kFlashBridgeRevision = 1;

	STATIC CONST Boolean kFlashEnabled			   = No;
	STATIC SizeT		 kFlashSize[kMaxFlash]	   = {};
	STATIC SizeT		 kFlashSectorSz[kMaxFlash] = {};

	/// @brief Enable flash memory builtin.
	STATIC Void drv_enable_flash(Int32 slot);

	/// @brief Disable flash memory builtin.
	STATIC Void drv_disable_flash(Int32 slot);

	/// @brief get sector count.
	/// @return drive sector count.
	SizeT drv_get_sector_count(Int32 slot)
	{
		if (slot > kMaxFlash)
			return 0;

		return kFlashSectorSz[slot];
	}

	/// @brief get device size.
	/// @return drive size
	SizeT drv_get_size(Int32 slot)
	{
		if (slot > kMaxFlash)
			return 0;

		return kFlashSize[slot];
	}

	/// @brief Enable flash memory at slot.
	STATIC Void drv_enable_flash(Int32 arg)
	{
		kcout << "Enabled FLSH hardware.\r";
	}

	/// @brief Disable flash memory at slot.
	STATIC Void drv_disable_flash(Int32 arg)
	{
		kcout << "Disabled FLSH hardware.\r";
	}
} // namespace Kernel

#endif // if ZKA_USE_MBCI_FLASH (Bridge)
