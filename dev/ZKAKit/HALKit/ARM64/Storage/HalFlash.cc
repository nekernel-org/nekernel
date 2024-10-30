/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <NewKit/Defines.h>
#include <ArchKit/ArchKit.h>

/// @file Flash.cc
/// @brief Flash memory builtin.

#ifdef __USE_MBCI_FLASH__

#define cMaxFlash (4U)

namespace Kernel
{
	/// /:/BRIDGE/FLSH/1
	constexpr auto kFlashBridgeMagic	= "FLSH";
	constexpr auto kFlashBridgeRevision = 1;

	STATIC const Boolean kFlashEnabled			   = No;
	STATIC SizeT		 kFlashSize[cMaxFlash]	   = {};
	STATIC SizeT		 kFlashSectorSz[cMaxFlash] = {};

	/// @brief Enable flash memory builtin.
	STATIC Void drv_enable_flash(Int32 slot);

	/// @brief Disable flash memory builtin.
	STATIC Void drv_disable_flash(Int32 slot);

	/// @brief get sector count.
	/// @return drive sector count.
	SizeT drv_get_sector_count(Int32 slot)
	{
		if (slot > cMaxFlash)
			return 0;

		return kFlashSectorSz[slot];
	}

	/// @brief get device size.
	/// @return drive size
	SizeT drv_get_size(Int32 slot)
	{
		if (slot > cMaxFlash)
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

#endif // if __USE_MBCI_FLASH__ (Bridge)
