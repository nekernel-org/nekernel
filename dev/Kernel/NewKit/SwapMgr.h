
/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define kSwapMgrBlockMaxSize mib_cast(16)
#define KSwapMgrBlockMagic "SWEP "

/// @file SwapMgr.h
/// @brief Virtual memory swap API.

namespace Kernel
{
    class UserSwapProcess;
}