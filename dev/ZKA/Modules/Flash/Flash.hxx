/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#ifdef __FLASH_MEM__

#include <NewKit/Defines.hxx>

/// @brief get sector count.
/// @return drive sector count.
Kernel::SizeT drv_std_get_sector_count();

/// @brief get device size.
/// @return drive size
Kernel::SizeT drv_std_get_drv_size();

#endif // ifdef __FLASH_MEM__
