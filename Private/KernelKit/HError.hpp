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

namespace HCore {
typedef Int32 HError;

inline constexpr HError kErrorSuccess = 0;
inline constexpr HError kErrorExecutable = 33;
inline constexpr HError kErrorExecutableLib = 34;
inline constexpr HError kErrorFileNotFound = 35;
inline constexpr HError kErrorDirectoryNotFound = 36;
inline constexpr HError kErrorDiskReadOnly = 37;
inline constexpr HError kErrorDiskIsFull = 38;
inline constexpr HError kErrorProcessFault = 39;
inline constexpr HError kErrorSocketHangUp = 40;
inline constexpr HError kErrorThreadLocalStorage = 41;
inline constexpr HError kErrorMath = 42;
inline constexpr HError kErrorNoNetwork = 43;
inline constexpr HError kErrorHeapOutOfMemory = 44;
inline constexpr HError kErrorNoSuchDisk = 45;

Boolean ke_bug_check(void) noexcept;
}  // namespace HCore

#define IsOk(HERR) (HERR == HCore::kErrorSuccess)
#define HasFailed(HERR) (HERR != HCore::kErrorSuccess)
#define GetLastError() LastError

inline HCore::HError LastError = 0;
