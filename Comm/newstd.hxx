/* -------------------------------------------

Copyright Zeta Electronics Corporation.

File: newstd.hxx.
Purpose: System Call Interface.

------------------------------------------- */

#ifndef _INC_COMM_NEWSTD_HXX_
#define _INC_COMM_NEWSTD_HXX_

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

#define ML_IMPORT_CXX extern "C++"
#define ML_IMPORT_C	  extern "C"

#define cRestrictR	"r"
#define cRestrictRB "rb"
#define cRestrictW	"w"
#define cRestrictRW "rw"

class NSyscall;		/// @brief System call class.

typedef int	 OSType;
typedef bool Bool;
typedef void UInt0;

typedef __UINT64_TYPE__ UInt64;
typedef __UINT32_TYPE__ UInt32;
typedef __UINT16_TYPE__ UInt16;
typedef __UINT8_TYPE__	UInt8;

typedef __SIZE_TYPE__ SizeT;

typedef __INT64_TYPE__ SInt64;
typedef __INT32_TYPE__ SInt32;
typedef __INT16_TYPE__ SInt16;
typedef __INT8_TYPE__  SInt8;

typedef char UTFChar;

typedef UInt32 MBCIType;

/**
	@brief Application class.
*/
class NSyscall
{
public:
	explicit NSyscall() = default;
	virtual ~NSyscall() = default;

	NSyscall& operator=(const NSyscall&) = default;
	NSyscall(const NSyscall&) = default;

public:
	/// @brief disable device.
	virtual UInt0 PowerOff(MBCIType)  = 0;

	/// @brief enable device.
	virtual UInt0 PowerOn(MBCIType)  = 0;

	/// @brief reboot device.
	virtual UInt0 PowerReboot(MBCIType)  = 0;

	/// @brief check if MBCI device is wokeup.
	virtual Bool  PowerIsWokeup(MBCIType)  = 0;

	/// @brief probe MBCI device from phone.
	virtual MBCIType PowerProbeDevice(const char* namepace, const int index)  = 0;

	// THOSE DOESNT REQUIRE PERMISSIONS FROM THE USER. //

	/// @brief terminate app.
	virtual UInt0 Terminate() = 0;

	/// @brief exit thread.
	virtual Bool Exit(OSType code) = 0;

	/// @brief alloc pointer.
	virtual UInt0* New(long long sz) = 0;

	/// @brief free pointer.
	virtual UInt0 Delete(void* ptr) = 0;

	// THOSE MAY REQUIRE PERMISSIONS FROM THE USER. //

	/// @brief Open descriptor.
	virtual OSType OpenStorage(const char* path, const char* restr) = 0;

	/// @brief Close descriptor.
	virtual UInt0 CloseStorage(OSType descriptorType) = 0;

	/// @brief Execute from shell.
	virtual OSType URLExecute(const UTFChar* shellLink) = 0;

	/// @brief Read descriptor.
	virtual UInt0* ReadStorage(const UTFChar* cmdNameOrData, SizeT cmdSize, OSType descriptorType) = 0;

	/// @brief Seek in storage file
	virtual UInt64 SeekStorage(OSType descriptorType, UInt64 offset) = 0;

	/// @brief Tell storage cursor.
	virtual UInt64 TellStorage(OSType descriptorType) = 0;

	/// @brief Remove stored file.
	virtual UInt64 RemoveStorage(OSType descriptorType) = 0;

	/// @brief Create stored file.
	virtual OSType CreateStorage(const UTFChar* fileName, UInt64 flags) = 0;

	/// @brief Write descriptor.
	virtual UInt0* WriteStorage(const UTFChar* cmdNameOrData, SizeT cmdSize, OSType descriptorType) = 0;

};

/// @brief Request syscall object.
/// @return Syscall implementation.
ML_IMPORT_C NSyscall* NRequestSyscall(UInt0);

/// @brief Release syscall object.
/// @param syscall System call object.
ML_IMPORT_C UInt0	  NReleaseSyscall(NSyscall* syscall);

#endif // ifndef _INC_COMM_NEWSTD_HXX_
