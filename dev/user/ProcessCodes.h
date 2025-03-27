/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <user/Macros.h>

/// @file ProcessCodes.h
/// @brief Process Codes type and values.

#define err_local_ok()	 (kLastError == kErrorSuccess)
#define err_local_fail() (kLastError != kErrorSuccess)
#define err_local_get()	 (kLastError)

typedef SInt32 ErrKind;

inline constexpr ErrKind kErrorSuccess			  = 0;
inline constexpr ErrKind kErrorExecutable		  = 33;
inline constexpr ErrKind kErrorExecutableLib	  = 34;
inline constexpr ErrKind kErrorFileNotFound		  = 35;
inline constexpr ErrKind kErrorDirectoryNotFound  = 36;
inline constexpr ErrKind kErrorDiskReadOnly		  = 37;
inline constexpr ErrKind kErrorDiskIsFull		  = 38;
inline constexpr ErrKind kErrorProcessFault		  = 39;
inline constexpr ErrKind kErrorSocketHangUp		  = 40;
inline constexpr ErrKind kErrorThreadLocalStorage = 41;
inline constexpr ErrKind kErrorMath				  = 42;
inline constexpr ErrKind kErrorNoNetwork		  = 43;
inline constexpr ErrKind kErrorHeapOutOfMemory	  = 44;
inline constexpr ErrKind kErrorNoSuchDisk		  = 45;
inline constexpr ErrKind kErrorFileExists		  = 46;
inline constexpr ErrKind kErrorFormatFailed		  = 47;
inline constexpr ErrKind kErrorNetworkTimeout	  = 48;
inline constexpr ErrKind kErrorInternal			  = 49;
inline constexpr ErrKind kErrorForkAlreadyExists  = 50;
inline constexpr ErrKind kErrorOutOfTeamSlot	  = 51;
inline constexpr ErrKind kErrorHeapNotPresent	  = 52;
inline constexpr ErrKind kErrorNoEntrypoint		  = 53;
inline constexpr ErrKind kErrorDiskIsCorrupted	  = 54;
inline constexpr ErrKind kErrorDisk				  = 55;
inline constexpr ErrKind kErrorInvalidData		  = 56;
inline constexpr ErrKind kErrorAsync			  = 57;
inline constexpr ErrKind kErrorNonBlocking		  = 58;
inline constexpr ErrKind kErrorIPC				  = 59;
inline constexpr ErrKind kErrorSign				  = 60;
inline constexpr ErrKind kErrorInvalidCreds		  = 61;
inline constexpr ErrKind kErrorCDTrayBroken		  = 62;
inline constexpr ErrKind kErrorUnrecoverableDisk  = 63;
inline constexpr ErrKind kErrorFileLocked		  = 64;
inline constexpr ErrKind kErrorUnimplemented	  = -1;

/// @brief The last error reported by the system to the process.
IMPORT_C ErrKind kLastError;
