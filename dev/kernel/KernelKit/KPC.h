/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>

/// @file KPC.h
/// @brief Kernel Procedure Code.

#define err_local_ok()                                                              \
  (Kernel::UserProcessScheduler::The().TheCurrentProcess().Leak().GetLocalCode() == \
   Kernel::kErrorSuccess)
#define err_local_fail()                                                            \
  (Kernel::UserProcessScheduler::The().TheCurrentProcess().Leak().GetLocalCode() != \
   Kernel::kErrorSuccess)
#define err_local_get() \
  (Kernel::UserProcessScheduler::The().TheCurrentProcess().Leak().GetLocalCode())

#define err_global_ok() (Kernel::kErrorLocalNumber == Kernel::kErrorSuccess)
#define err_global_fail() (Kernel::kErrorLocalNumber != Kernel::kErrorSuccess)
#define err_global_get() (Kernel::kErrorLocalNumber)

namespace Kernel {
typedef Int32 KPCError;

inline KPCError kErrorLocalNumber = 0UL;

inline constexpr KPCError kErrorSuccess            = 0;
inline constexpr KPCError kErrorExecutable         = 33;
inline constexpr KPCError kErrorExecutableLib      = 34;
inline constexpr KPCError kErrorFileNotFound       = 35;
inline constexpr KPCError kErrorDirectoryNotFound  = 36;
inline constexpr KPCError kErrorDiskReadOnly       = 37;
inline constexpr KPCError kErrorDiskIsFull         = 38;
inline constexpr KPCError kErrorProcessFault       = 39;
inline constexpr KPCError kErrorSocketHangUp       = 40;
inline constexpr KPCError kErrorThreadLocalStorage = 41;
inline constexpr KPCError kErrorMath               = 42;
inline constexpr KPCError kErrorNoNetwork          = 43;
inline constexpr KPCError kErrorHeapOutOfMemory    = 44;
inline constexpr KPCError kErrorNoSuchDisk         = 45;
inline constexpr KPCError kErrorFileExists         = 46;
inline constexpr KPCError kErrorFormatFailed       = 47;
inline constexpr KPCError kErrorNetworkTimeout     = 48;
inline constexpr KPCError kErrorInternal           = 49;
inline constexpr KPCError kErrorForkAlreadyExists  = 50;
inline constexpr KPCError kErrorOutOfTeamSlot      = 51;
inline constexpr KPCError kErrorHeapNotPresent     = 52;
inline constexpr KPCError kErrorNoEntrypoint       = 53;
inline constexpr KPCError kErrorDiskIsCorrupted    = 54;
inline constexpr KPCError kErrorDisk               = 55;
inline constexpr KPCError kErrorInvalidData        = 56;
inline constexpr KPCError kErrorAsync              = 57;
inline constexpr KPCError kErrorNonBlocking        = 58;
inline constexpr KPCError kErrorIPC                = 59;
inline constexpr KPCError kErrorSign               = 60;
inline constexpr KPCError kErrorInvalidCreds       = 61;
inline constexpr KPCError kErrorCDTrayBroken       = 62;
inline constexpr KPCError kErrorUnrecoverableDisk  = 63;
inline constexpr KPCError kErrorFileLocked         = 64;
inline constexpr KPCError kErrorDiskIsTooTiny      = 65;
inline constexpr KPCError kErrorDmaExhausted       = 66;
inline constexpr KPCError kErrorOutOfBitMapMemory  = 67;
inline constexpr KPCError kErrorTimeout     = 68;
/// Generic errors.
inline constexpr KPCError kErrorUnimplemented = -1;

/// @brief Does a system wide bug check.
/// @param void no params are needed.
/// @return if error-free: false, otherwise true.
Boolean err_bug_check_raise(Void) noexcept;
}  // namespace Kernel
