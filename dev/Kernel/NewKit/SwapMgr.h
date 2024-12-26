
/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

#define kSwapMgrBlockMaxSize mib_cast(16)
#define KSwapMgrBlockMagic "SWEP "

#define kSwapMgrPageFile "/System/pagefile.sys"

/// @file SwapMgr.h
/// @brief Virtual memory swap manager.

namespace Kernel
{
    class SwapMgrDisk;
    
    class SwapMgrDisk
    {
        static BOOL DumpToDisk(const Char* fork_name, SizeT fork_name_len = 0)
        {
            return YES;
        }
    }
}