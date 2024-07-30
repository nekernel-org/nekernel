/* -------------------------------------------

Copyright ZKA Technologies.

File: Types.hxx.
Purpose: System Call types.

------------------------------------------- */


#pragma once

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

#define cRestrictR	1
#define cRestrictRB 2
#define cRestrictW	4
#define cRestrictRW 6

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

// Interfaces are divided between classes.
// So that they aren't too big.

class SharedInterface1;
class SharedInterface2;
class SharedInterface3;
class UnknownInterface;
class EncodingInterface;

class __attribute__((uuid_of(UnknownInterface))) UnknownInterface
{
public:
	explicit UnknownInterface() = default;
	virtual ~UnknownInterface() = default;

	UnknownInterface& operator=(const UnknownInterface&) = default;
	UnknownInterface(const UnknownInterface&)			 = default;

	SInt32 Release() { return -1; }
	
	template <typename TCLS>
    SInt32 Release(TCLS* cls) { return -1; }

	template <typename TCLS, typename UCLSID>
	TCLS* QueryInterface(UCLSID uclsidOfCls) { return nullptr; }
};

template <typename TCLS, typename UCLSID>
TCLS* SciGetClassFromCLSID(UCLSID uclsidOfCls);