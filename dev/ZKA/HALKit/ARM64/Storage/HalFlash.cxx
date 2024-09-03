/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <NewKit/Defines.hxx>
#include <ArchKit/ArchKit.hxx>

/// @file Flash.cxx
/// @brief Flash memory builtin.

#ifdef __USE_MBCI_FLASH__

#define cMaxFlash (4U)

namespace Kernel
{
	/// /:\\BRIDGE\\FLSH\\1
	constexpr auto cFlashBridgeMagic	= "FLSH";
	constexpr auto cFlashBridgeRevision = 1;

	STATIC const Boolean kFlashEnabled			   = No;
	STATIC SizeT		 kFlashSize[cMaxFlash]	   = {};
	STATIC SizeT		 kFlashSectorSz[cMaxFlash] = {};

	/// @brief Enable flash memory builtin.
	STATIC Void drv_enable_flash(Int32 slot);

	/// @brief Disable flash memory builtin.
	STATIC Void drv_disable_flash(Int32 slot);

	/// @brief get sector count.
	/// @return drive sector count.
	SizeT drv_std_get_sector_count(Int32 slot)
	{
		if (slot > cMaxFlash)
			return 0;

		return kFlashSectorSz[slot];
	}

	/// @brief get device size.
	/// @return drive size
	SizeT drv_std_get_drv_size(Int32 slot)
	{
		if (slot > cMaxFlash)
			return 0;

		return kFlashSize[slot];
	}

	/// @brief Enable flash memory at slot.
	STATIC Void drv_enable_flash(Int32 arg)
	{
		kcout << "newoskrnl.exe: Enabled FLSH hardware.\r";
	}

	/// @brief Disable flash memory at slot.
	STATIC Void drv_disable_flash(Int32 arg)
	{
		kcout << "newoskrnl.exe: Disabled FLSH hardware.\r";
	}
} // namespace Kernel

#endif // if __USE_MBCI_FLASH__ (Bridge)
