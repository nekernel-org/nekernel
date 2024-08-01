/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <SCIKit/SCIBase.hxx>

#define ErrLocalIsOk()	 (kLastError == kErrorSuccess)
#define ErrLocalFailed() (kLastError != kErrorSuccess)
#define ErrLocal()		 (kLastError)

typedef SInt32 NEW_ERROR_TYPE;

inline constexpr NEW_ERROR_TYPE kErrorSuccess			 = 0;
inline constexpr NEW_ERROR_TYPE kErrorExecutable		 = 33;
inline constexpr NEW_ERROR_TYPE kErrorExecutableLib		 = 34;
inline constexpr NEW_ERROR_TYPE kErrorFileNotFound		 = 35;
inline constexpr NEW_ERROR_TYPE kErrorDirectoryNotFound	 = 36;
inline constexpr NEW_ERROR_TYPE kErrorDiskReadOnly		 = 37;
inline constexpr NEW_ERROR_TYPE kErrorDiskIsFull		 = 38;
inline constexpr NEW_ERROR_TYPE kErrorProcessFault		 = 39;
inline constexpr NEW_ERROR_TYPE kErrorSocketHangUp		 = 40;
inline constexpr NEW_ERROR_TYPE kErrorThreadLocalStorage = 41;
inline constexpr NEW_ERROR_TYPE kErrorMath				 = 42;
inline constexpr NEW_ERROR_TYPE kErrorNoNetwork			 = 43;
inline constexpr NEW_ERROR_TYPE kErrorHeapOutOfMemory	 = 44;
inline constexpr NEW_ERROR_TYPE kErrorNoSuchDisk		 = 45;
inline constexpr NEW_ERROR_TYPE kErrorFileExists		 = 46;
inline constexpr NEW_ERROR_TYPE kErrorFormatFailed		 = 47;
inline constexpr NEW_ERROR_TYPE kErrorNetworkTimeout	 = 48;
inline constexpr NEW_ERROR_TYPE kErrorInternal			 = 49;
inline constexpr NEW_ERROR_TYPE kErrorForkAlreadyExists	 = 50;
inline constexpr NEW_ERROR_TYPE kErrorOutOfTeamSlot		 = 51;
inline constexpr NEW_ERROR_TYPE kErrorHeapNotPresent	 = 52;
inline constexpr NEW_ERROR_TYPE kErrorNoEntrypoint		 = 53;
inline constexpr NEW_ERROR_TYPE kErrorDiskIsCorrupted	 = 54;
inline constexpr NEW_ERROR_TYPE kErrorDisk				 = 55;
inline constexpr NEW_ERROR_TYPE kErrorInvalidData		 = 56;
inline constexpr NEW_ERROR_TYPE kErrorAsync				 = 57;
inline constexpr NEW_ERROR_TYPE kErrorNonBlocking		 = 58;
inline constexpr NEW_ERROR_TYPE kErrorIPC				 = 59;
inline constexpr NEW_ERROR_TYPE kErrorSign				 = 60;
inline constexpr NEW_ERROR_TYPE kErrorInvalidCreds		 = 61;
inline constexpr NEW_ERROR_TYPE kErrorUnimplemented		 = 0;

IMPORT_C NEW_ERROR_TYPE kLastError;
