/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef NE_USE_MBCI_FLASH

#include <NewKit/Defines.h>
#include <ArchKit/ArchKit.h>
#include <Mod/MFlash/MFlash.h>
#include <Mod/MBCI/MBCI.h>

/// @file MFlash.cc
/// @brief MBCI Flash support.

#define kMaxFlashSlots (8U)

namespace Kernel
{
	/// /Mount/Flash/n
	constexpr auto kFlashBridgeMagic	= "FLSH";
	constexpr auto kFlashBridgeRevision = 1;

	STATIC BOOL		  kFlashEnabled						= NO;
	STATIC SizeT	  kFlashSize[kMaxFlashSlots]		= {};
	STATIC SizeT	  kFlashSectorSz[kMaxFlashSlots]	= {};
	STATIC IMBCIHost* kFlashMetaPackets[kMaxFlashSlots] = {};
	STATIC IMBCIHost* kFlashDataPackets[kMaxFlashSlots] = {};

	/// @brief Enable flash memory builtin.
	STATIC Void drv_enable_flash(Int32 slot);

	/// @brief Disable flash memory builtin.
	STATIC Void drv_disable_flash(Int32 slot);

	/// @brief get slot sector count.
	/// @return slot sector count.
	SizeT drv_get_sector_count(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return 0;

		return kFlashSectorSz[slot];
	}

	/// @brief get slot full size (in bytes).
	/// @return drive slot size
	SizeT drv_get_size(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return 0;

		return kFlashSize[slot];
	}

	/// @brief Enable flash memory at slot.
	BOOL drv_enable_at(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return NO;

		kFlashMetaPackets[slot]->InterruptEnable = YES;

		kout << "Enabled hardware slot at: " << number(slot) << endl;

		return YES;
	}

	/// @brief Disable flash memory at slot.
	BOOL drv_disable_at(Int32 slot)
	{
		if (slot > kMaxFlashSlots)
			return NO;

		kFlashMetaPackets[slot]->InterruptEnable = NO;

		kout << "Disabled hardware slot at: " << number(slot) << endl;

		return YES;
	}
} // namespace Kernel

#endif // if NE_USE_MBCI_FLASH
