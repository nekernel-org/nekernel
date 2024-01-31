/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

namespace HCore
{
    typedef Int32 OSErr;

    inline constexpr OSErr kErrorExecutable = 33;
    inline constexpr OSErr kErrorExecutableLib = 34;
    inline constexpr OSErr kErrorFileNotFound = 35;
    inline constexpr OSErr kErrorDirectoryNotFound = 36;
    inline constexpr OSErr kErrorDiskReadOnly = 37;
    inline constexpr OSErr kErrorDiskIsFull = 38;
    inline constexpr OSErr kErrorProcessFault = 39;
    inline constexpr OSErr kErrorSocketHangUp = 40;
    inline constexpr OSErr kErrorThreadLocalStorage = 41;
    inline constexpr OSErr kErrorMath = 42;
    inline constexpr OSErr kErrorNoNetwork = 43;
    inline constexpr OSErr kErrorHeapOutOfMemory = 44;
}