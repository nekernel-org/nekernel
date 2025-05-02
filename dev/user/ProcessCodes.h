/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <user/Macros.h>

/// @file ProcessCodes.h
/// @brief Process Codes type and values.
/// @author Amlal El Mahrouss (amlal@nekernel.org)

#define err_local_ok() (kLastError == kErrorSuccess)
#define err_local_fail() (kLastError != kErrorSuccess)
#define err_local_get() (kLastError)

typedef SInt32 ErrRef;

inline constexpr ErrRef kErrorSuccess            = 0;
inline constexpr ErrRef kErrorExecutable         = 33;
inline constexpr ErrRef kErrorExecutableLib      = 34;
inline constexpr ErrRef kErrorFileNotFound       = 35;
inline constexpr ErrRef kErrorDirectoryNotFound  = 36;
inline constexpr ErrRef kErrorDiskReadOnly       = 37;
inline constexpr ErrRef kErrorDiskIsFull         = 38;
inline constexpr ErrRef kErrorProcessFault       = 39;
inline constexpr ErrRef kErrorSocketHangUp       = 40;
inline constexpr ErrRef kErrorThreadLocalStorage = 41;
inline constexpr ErrRef kErrorMath               = 42;
inline constexpr ErrRef kErrorNoNetwork          = 43;
inline constexpr ErrRef kErrorHeapOutOfMemory    = 44;
inline constexpr ErrRef kErrorNoSuchDisk         = 45;
inline constexpr ErrRef kErrorFileExists         = 46;
inline constexpr ErrRef kErrorFormatFailed       = 47;
inline constexpr ErrRef kErrorNetworkTimeout     = 48;
inline constexpr ErrRef kErrorInternal           = 49;
inline constexpr ErrRef kErrorForkAlreadyExists  = 50;
inline constexpr ErrRef kErrorOutOfTeamSlot      = 51;
inline constexpr ErrRef kErrorHeapNotPresent     = 52;
inline constexpr ErrRef kErrorNoEntrypoint       = 53;
inline constexpr ErrRef kErrorDiskIsCorrupted    = 54;
inline constexpr ErrRef kErrorDisk               = 55;
inline constexpr ErrRef kErrorInvalidData        = 56;
inline constexpr ErrRef kErrorAsync              = 57;
inline constexpr ErrRef kErrorNonBlocking        = 58;
inline constexpr ErrRef kErrorIPC                = 59;
inline constexpr ErrRef kErrorSign               = 60;
inline constexpr ErrRef kErrorInvalidCreds       = 61;
inline constexpr ErrRef kErrorCDTrayBroken       = 62;
inline constexpr ErrRef kErrorUnrecoverableDisk  = 63;
inline constexpr ErrRef kErrorFileLocked         = 64;
inline constexpr ErrRef kErrorDiskIsTooTiny      = 65;
inline constexpr ErrRef kErrorUnimplemented      = -1;

/// @brief The last error reported by the system to the process.
IMPORT_C ErrRef kLastError;
