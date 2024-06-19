/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#ifdef __FLASH_MEM__

/// @brief get sector count.
NewOS::SizeT drv_std_get_sector_count();

/// @brief get device size.
NewOS::SizeT drv_std_get_drv_size();

#endif // ifdef __FLASH_MEM__
