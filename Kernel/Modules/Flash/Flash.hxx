/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#ifdef __FLASH_MEM__

/// @brief get sector count.
/// @return drive sector count.
NewOS::SizeT drv_std_get_sector_count();

/// @brief get device size.
/// @return drive size
NewOS::SizeT drv_std_get_drv_size();

#endif // ifdef __FLASH_MEM__
