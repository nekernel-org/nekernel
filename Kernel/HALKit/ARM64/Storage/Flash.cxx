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
	SizeT drv_std_get_sector_count(void)
	{
		return 0;
	}

	SizeT drv_std_get_drv_size(void)
	{
		return 0;
	}
} // namespace NewOS

#endif // if __FLASH_MEM__ (Bridge)
