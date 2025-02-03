/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

/// @file LPC.h
/// @brief Local Process Codes.

#define err_local_ok()	 (Kernel::UserProcessScheduler::The().CurrentProcess().Leak().GetLocalCode() == Kernel::kErrorSuccess)
#define err_local_fail() (Kernel::UserProcessScheduler::The().CurrentProcess().Leak().GetLocalCode() != Kernel::kErrorSuccess)
#define err_local_get()	 (Kernel::UserProcessScheduler::The().CurrentProcess().Leak().GetLocalCode())

#define err_global_ok()	  (Kernel::kErrorLocalNumber == Kernel::kErrorSuccess)
#define err_global_fail() (Kernel::kErrorLocalNumber != Kernel::kErrorSuccess)
#define err_global_get()  (Kernel::kErrorLocalNumber)

namespace Kernel
{
	typedef Int32 HError;

	inline HError kErrorLocalNumber = 0UL;

	inline constexpr HError kErrorSuccess			 = 0;
	inline constexpr HError kErrorExecutable		 = 33;
	inline constexpr HError kErrorExecutableLib		 = 34;
	inline constexpr HError kErrorFileNotFound		 = 35;
	inline constexpr HError kErrorDirectoryNotFound	 = 36;
	inline constexpr HError kErrorDiskReadOnly		 = 37;
	inline constexpr HError kErrorDiskIsFull		 = 38;
	inline constexpr HError kErrorProcessFault		 = 39;
	inline constexpr HError kErrorSocketHangUp		 = 40;
	inline constexpr HError kErrorThreadLocalStorage = 41;
	inline constexpr HError kErrorMath				 = 42;
	inline constexpr HError kErrorNoNetwork			 = 43;
	inline constexpr HError kErrorHeapOutOfMemory	 = 44;
	inline constexpr HError kErrorNoSuchDisk		 = 45;
	inline constexpr HError kErrorFileExists		 = 46;
	inline constexpr HError kErrorFormatFailed		 = 47;
	inline constexpr HError kErrorNetworkTimeout	 = 48;
	inline constexpr HError kErrorInternal			 = 49;
	inline constexpr HError kErrorForkAlreadyExists	 = 50;
	inline constexpr HError kErrorOutOfTeamSlot		 = 51;
	inline constexpr HError kErrorHeapNotPresent	 = 52;
	inline constexpr HError kErrorNoEntrypoint		 = 53;
	inline constexpr HError kErrorDiskIsCorrupted	 = 54;
	inline constexpr HError kErrorDisk				 = 55;
	inline constexpr HError kErrorInvalidData		 = 56;
	inline constexpr HError kErrorAsync				 = 57;
	inline constexpr HError kErrorNonBlocking		 = 58;
	inline constexpr HError kErrorIPC				 = 59;
	inline constexpr HError kErrorSign				 = 60;
	inline constexpr HError kErrorInvalidCreds		 = 61;
	inline constexpr HError kErrorCDTrayBroken		 = 62;
	inline constexpr HError kErrorUnrecoverableDisk	 = 63;
	inline constexpr HError kErrorFileLocked		 = 64;
	inline constexpr HError kErrorUnimplemented		 = 0;

	/// @brief Raises a bug check stop code.
	Void err_bug_check_raise(Void) noexcept;

	/// @brief Does a system wide bug check.
	/// @param void no params are needed.
	/// @return if error-free: false, otherwise true.
	Boolean err_bug_check(Void) noexcept;
} // namespace Kernel
