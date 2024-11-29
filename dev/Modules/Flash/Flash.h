/* -------------------------------------------

	Copyright (C) 2024, ELMH Group, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef __USE_MBCI_FLASH__

#include <NewKit/Defines.h>

/// @brief get sector count.
/// @return drive sector count.
Kernel::SizeT drv_get_sector_count();

/// @brief get device size.
/// @return drive size
Kernel::SizeT drv_get_size();

#endif // ifdef __USE_MBCI_FLASH__
