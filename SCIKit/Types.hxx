/* -------------------------------------------

Copyright ZKA Technologies.

File: Types.hxx.
Purpose: System Call types.

------------------------------------------- */

#pragma once

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

typedef bool		  Bool;
typedef void		  UInt0;

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
class UnknownUCLSID;	// From the IDB, the constructor of the object, e.g: WordUCLSID.

class __attribute__((uuid("d7c144b6-0792-44b8-b06b-02b227b547df"))) UnknownInterface
{
public:
	explicit UnknownInterface() = default;
	virtual ~UnknownInterface() = default;

	UnknownInterface& operator=(const UnknownInterface&) = default;
	UnknownInterface(const UnknownInterface&)			 = default;

	SInt32 Release()
	{
		delete this;
		return 0;
	}
};

#ifdef __NEWOS_SYMS__
#include <SCIKit/rtl.internal.inl>
#else

/// @brief Allocate new SCM class.
/// @tparam TCLS 
/// @tparam UCLSID 
/// @param uclsidOfCls 
/// @return 
template <typename TCLS, typename UCLSID, typename... Args>
inline TCLS* RtlGetClassFromCLSID(UCLSID uclsidOfCls, Args... args);

/// @brief Release SCM class.
/// @tparam TCLS 
/// @param cls 
/// @return 
template <typename TCLS>
inline SInt32 RtlReleaseClass(TCLS* cls);

#endif