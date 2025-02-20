/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#ifdef NE_USE_MBCI_FLASH

#include <NewKit/Defines.h>

NeOS::SizeT drv_get_sector_count(NeOS::Int32 slot);

NeOS::SizeT drv_get_size(NeOS::Int32 slot);

NeOS::Void drv_enable_at(NeOS::Int32 slot);

NeOS::Void drv_disable_at(NeOS::Int32 slot);

#endif // ifdef NE_USE_MBCI_FLASH
