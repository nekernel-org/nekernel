/* -------------------------------------------

Copyright ZKA Technologies.

File: StandardInterface1.hxx.
Purpose: System Call Interface Version 1.

------------------------------------------- */

#ifndef __SHARED_INTERFACE_HXX__
#define __SHARED_INTERFACE_HXX__

#define cRestrictR	1
#define cRestrictRB 2
#define cRestrictW	4
#define cRestrictRW 6

#include <SCIKit/Types.hxx>

typedef UInt32 PowerID;
typedef long long int FD;

/**
	@brief System call class.
*/
class __attribute__((uuid("21f40aef-cce0-4c0b-9672-40f9053394bc"))) SharedInterface1 : public UnknownInterface
{
public:
	explicit SharedInterface1() = default;
	virtual ~SharedInterface1() = default;

	SharedInterface1& operator=(const SharedInterface1&) = default;
	SharedInterface1(const SharedInterface1&)			 = default;

public:
	/// @brief disable device.
	virtual UInt0 PowerOff(PowerID) = 0;

	/// @brief enable device.
	virtual UInt0 PowerOn(PowerID) = 0;

	/// @brief reboot device.
	virtual UInt0 PowerReboot(PowerID) = 0;

	/// @brief check if MBCI device is wokeup.
	virtual Bool PowerIsWokeup(PowerID) = 0;

	/// @brief probe MBCI/ACPI device from device.
	virtual PowerID PowerProbeDevice(const char* namezpace, const int index) = 0;

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
	virtual FD OpenStorage(const char* path, const UInt32 restr) = 0;

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

#endif // ifndef __SHARED_INTERFACE_HXX__
