/* -------------------------------------------

Copyright Zeta Electronics Corporation.

File: newstd.hxx.
Purpose: System Call Interface.

------------------------------------------- */

#pragma once

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

#define ML_IMPORT_CXX extern "C++"
#define ML_IMPORT_C	  extern "C"

#define cRestrictR	"r"
#define cRestrictRB "rb"
#define cRestrictW	"w"
#define cRestrictRW "rw"

class NUser;		/// @brief User application class.
class NWindow;		/// @brief Window class.
class NWindowAlert; /// @brief Window alert object
class NURL;			/// @brief URL object.

typedef void (*NWindowCallback)(NWindow*);

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

/**
	@brief Application class.
*/
class NApplication
{
public:
	explicit NApplication() = default;
	virtual ~NApplication() = default;

	typedef UInt32 MBCIType;

public:
	/// @brief disable device.
	UInt0 PowerOff(MBCIType);

	/// @brief enable device.
	UInt0 PowerOn(MBCIType);

	/// @brief reboot device.
	UInt0 PowerReboot(MBCIType);

	/// @brief check if MBCI device is wokeup.
	Bool  PowerIsWokeup(MBCIType);

	/// @brief probe MBCI device from phone.
	MBCIType PowerProbeDevice(const char* namepace, const int index);

	// THOSE DOESNT REQUIRE PERMISSIONS FROM THE USER. //

	/// @brief terminate app.
	virtual UInt0  AppTerminate() = 0;

	/// @brief exit thread.
	virtual Bool   ThreadExit(OSType code) = 0;

	/// @brief alloc pointer.
	virtual UInt0* ProcessNew(long long sz) = 0;

	/// @brief free pointer.
	virtual UInt0  ProcessDelete(void* ptr) = 0;

	// THOSE MAY REQUIRE PERMISSIONS FROM THE USER. //

	/// @brief Open descriptor.
	virtual OSType OpenStorage(const char* path, const char* restr) = 0;

	/// @brief Close descriptor.
	virtual UInt0  CloseStorage(OSType descriptorType) = 0;

	/// @brief Execute from shell.
	virtual NURL*  URLExecute(const NURL* shell) = 0;

	/// @brief Read descriptor.
	virtual UInt0* ReadStorage(const UTFChar* cmdNameOrData, SizeT cmdSize, OSType descriptorType) = 0;

	/// @brief Write descriptor.
	virtual UInt0* WriteStorage(const UTFChar* cmdNameOrData, SizeT cmdSize, OSType descriptorType) = 0;

};

/**
@brief This class contains an URL
*/
class NURL
{
public:
	explicit NURL() = default;
	virtual ~NURL() = default;

public:
	virtual NURL* Navigate(const char* url)		 = 0;
	virtual NURL* Protocol(const char* protocol) = 0;

public:
	char* fBufferPtr;
	long  fBufferLen;
};
