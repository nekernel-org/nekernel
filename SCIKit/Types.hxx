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

// Interfaces are divided between classes.
// So that they aren't too big.

class UnknownInterface; // Refrenced from an IDB entry.
class UnknownUCLSID; // From IDB, the constructor of the object.

class __attribute__((uuid("d7c144b6-0792-44b8-b06b-02b227b547df"))) UnknownInterface
{
public:
	explicit UnknownInterface() = default;
	virtual ~UnknownInterface() = default;

	UnknownInterface& operator=(const UnknownInterface&) = default;
	UnknownInterface(const UnknownInterface&)			 = default;

	SInt32 Release() { delete this; }
	
	template <typename TCLS>
    SInt32 Release(TCLS* cls) { delete cls; return 0; }

	template <typename TCLS, typename UCLSID, typename... Args>
	TCLS* QueryInterface(UCLSID uclsidOfCls, Args... args) { return uclsidOfCls->QueryObject<TCLS>(args...); }
};

template <typename TCLS, typename UCLSID>
TCLS* RtlGetClassFromCLSID(UCLSID uclsidOfCls);