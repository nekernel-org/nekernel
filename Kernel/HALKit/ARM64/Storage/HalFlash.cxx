/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <NewKit/Defines.hpp>
#include <ArchKit/ArchKit.hpp>

/// @file Flash.cxx
/// @brief Flash memory builtin.

#ifdef __FLASH_MEM__

namespace NewOS
{
    /// /:\\BRIDGE\\FLSH\\1
    constexpr auto cFlashBridgeMagic = "FLSH";
    constexpr auto cFlashBridgeRevision = 1;

    STATIC const Boolean kFlashEnabled = No;
    STATIC SizeT kFlashSize = 0UL;
    STATIC SizeT kFlashSectorSz = 0UL;

    /// @brief Enable flash memory builtin.
    STATIC Void drv_enable_builtin(Void);

    /// @brief Disable flash memory builtin.
    STATIC Void drv_disable_builtin(Void);

    /// @brief get sector count.
    /// @return drive sector count.
	SizeT drv_std_get_sector_count(void)
	{
		return kFlashSectorSz;
	}

	/// @brief get device size.
	/// @return drive size
	SizeT drv_std_get_drv_size(void)
	{
		return kFlashSize;
	}

	/// @brief Enable flash memory builtin.
    STATIC Void drv_enable_builtin(Void) {}

    /// @brief Disable flash memory builtin.
    STATIC Void drv_disable_builtin(Void) {}
} // namespace NewOS

#endif // if __FLASH_MEM__ (Bridge)
