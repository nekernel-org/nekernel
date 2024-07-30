/* -------------------------------------------

Copyright ZKA Technologies.

File: newstd.hxx.
Purpose: System Call Interface.

------------------------------------------- */

#ifndef __SCI_STD_HXX__
#define __SCI_STD_HXX__

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

#define cRestrictR	"r"
#define cRestrictRB "rb"
#define cRestrictW	"w"
#define cRestrictRW "rw"

class SCISharedInterface; /// @brief System call class.

typedef long long int	 FD;
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

typedef UInt32 PowerID;

/**
	@brief System call class.
*/
class SCISharedInterface
{
public:
	explicit SCISharedInterface() = default;
	virtual ~SCISharedInterface() = default;

	SCISharedInterface& operator=(const SCISharedInterface&) = default;
	SCISharedInterface(const SCISharedInterface&)			 = default;

public:
	/// @brief disable device.
	virtual UInt0 PowerOff(PowerID) = 0;

	/// @brief enable device.
	virtual UInt0 PowerOn(PowerID) = 0;

	/// @brief reboot device.
	virtual UInt0 PowerReboot(PowerID) = 0;

	/// @brief check if MBCI device is wokeup.
	virtual Bool PowerIsWokeup(PowerID) = 0;

	/// @brief probe MBCI/ACPI device from phone.
	virtual PowerID PowerProbeDevice(const char* namepace, const int index) = 0;

	// THOSE DOESNT REQUIRE PERMISSIONS FROM THE USER. //

	/// @brief terminate app.
	virtual UInt0 Terminate() = 0;

	/// @brief exit thread.
	virtual Bool Exit(FD code) = 0;

	/// @brief alloc pointer.
	virtual UInt0* New(long long sz) = 0;

	/// @brief free pointer.
	virtual UInt0 Delete(void* ptr) = 0;

	// THOSE MAY REQUIRE PERMISSIONS FROM THE USER. //

	/// @brief Open descriptor.
	virtual FD OpenStorage(const char* path, const char* restr) = 0;

	/// @brief Close descriptor.
	virtual UInt0 CloseStorage(FD descriptorType) = 0;

	/// @brief Execute from shell.
	virtual FD URLExecute(const UTFChar* shellLink) = 0;

	/// @brief Read descriptor.
	virtual UInt0* ReadStorage(const UTFChar* cmdNameOrData, SizeT cmdSize, FD descriptorType) = 0;

	/// @brief Seek in storage file
	virtual UInt64 SeekStorage(FD descriptorType, UInt64 offset) = 0;

	/// @brief Tell storage cursor.
	virtual UInt64 TellStorage(FD descriptorType) = 0;

	/// @brief Remove stored file.
	virtual UInt64 RemoveStorage(FD descriptorType) = 0;

	/// @brief Create stored file.
	virtual FD CreateStorage(const UTFChar* fileName, UInt64 flags) = 0;

	/// @brief Write descriptor.
	virtual UInt0* WriteStorage(const UTFChar* cmdNameOrData, SizeT cmdSize, FD descriptorType) = 0;
};

/// @brief Get shared syscall object.
/// @return Syscall implementation.
IMPORT_C SCISharedInterface* SCIGetSharedInterface(UInt0);

#endif // ifndef __SCI_STD_HXX__
