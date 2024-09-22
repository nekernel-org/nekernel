/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#define ErrLocalIsOk()	 (kLastError == kErrorSuccess)
#define ErrLocalFailed() (kLastError != kErrorSuccess)
#define ErrLocal()		 (kLastError)

typedef SInt32 ZKAErr;

inline constexpr ZKAErr kErrorSuccess			 = 0;
inline constexpr ZKAErr kErrorExecutable		 = 33;
inline constexpr ZKAErr kErrorExecutableLib		 = 34;
inline constexpr ZKAErr kErrorFileNotFound		 = 35;
inline constexpr ZKAErr kErrorDirectoryNotFound	 = 36;
inline constexpr ZKAErr kErrorDiskReadOnly		 = 37;
inline constexpr ZKAErr kErrorDiskIsFull		 = 38;
inline constexpr ZKAErr kErrorProcessFault		 = 39;
inline constexpr ZKAErr kErrorSocketHangUp		 = 40;
inline constexpr ZKAErr kErrorThreadLocalStorage = 41;
inline constexpr ZKAErr kErrorMath				 = 42;
inline constexpr ZKAErr kErrorNoNetwork			 = 43;
inline constexpr ZKAErr kErrorHeapOutOfMemory	 = 44;
inline constexpr ZKAErr kErrorNoSuchDisk		 = 45;
inline constexpr ZKAErr kErrorFileExists		 = 46;
inline constexpr ZKAErr kErrorFormatFailed		 = 47;
inline constexpr ZKAErr kErrorNetworkTimeout	 = 48;
inline constexpr ZKAErr kErrorInternal			 = 49;
inline constexpr ZKAErr kErrorForkAlreadyExists	 = 50;
inline constexpr ZKAErr kErrorOutOfTeamSlot		 = 51;
inline constexpr ZKAErr kErrorHeapNotPresent	 = 52;
inline constexpr ZKAErr kErrorNoEntrypoint		 = 53;
inline constexpr ZKAErr kErrorDiskIsCorrupted	 = 54;
inline constexpr ZKAErr kErrorDisk				 = 55;
inline constexpr ZKAErr kErrorInvalidData		 = 56;
inline constexpr ZKAErr kErrorAsync				 = 57;
inline constexpr ZKAErr kErrorNonBlocking		 = 58;
inline constexpr ZKAErr kErrorIPC				 = 59;
inline constexpr ZKAErr kErrorSign				 = 60;
inline constexpr ZKAErr kErrorInvalidCreds		 = 61;
inline constexpr ZKAErr kErrorUnimplemented		 = 0;

IMPORT_C ZKAErr kLastError;
