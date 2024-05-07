/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>

namespace NewOS
{
	typedef Int32 HError;

	inline constexpr HError kErrorSuccess			 = 0;
	inline constexpr HError kErrorExecutable		 = 33;
	inline constexpr HError kErrorExecutableLib		 = 34; // no such library!!!
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
	inline constexpr HError kErrorUnimplemented		 = 0;

	Boolean ke_bug_check(void) noexcept;
} // namespace NewOS

#define DbgOk()		   (kLastError == NewOS::kErrorSuccess)
#define DbgFailed()	   (kLastError != NewOS::kErrorSuccess)
#define DbgLastError() kLastError

inline NewOS::HError kLastError = 0;
