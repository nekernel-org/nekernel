/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef ZKA_USE_MBCI_FLASH

#include <NewKit/Defines.h>

/// @brief get sector count.
/// @return drive sector count.
Kernel::SizeT drv_get_sector_count();

/// @brief get device size.
/// @return drive size
Kernel::SizeT drv_get_size();

#endif // ifdef ZKA_USE_MBCI_FLASH
