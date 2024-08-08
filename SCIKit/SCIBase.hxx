/* -------------------------------------------

Copyright ZKA Technologies.

File: SCIBase.hxx
Purpose: SCIKit foundation header.

------------------------------------------- */

#pragma once

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

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

typedef void*			 VoidPtr;
typedef __UINTPTR_TYPE__ UIntPtr;
typedef char			 Char;

#ifdef __NEWOS_SYMS__
#include <SCIKit/scm.internal.inl>
#else
class UnknownInterface; // Refrenced from an IDB entry.
class UnknownUCLSID;	// From the IDB, the constructor of the object, e.g: WordUCLSID.
class UUID;

/// @brief Allocate new SCM class.
/// @tparam TCLS
/// @tparam UCLSID
/// @param uclsidOfCls
/// @return
template <typename TCLS, typename UCLSID, typename... Args>
TCLS* RtlQueryInterface(UCLSID uclsidOfCls, Args... args);

/// @brief Release SCM class.
/// @tparam TCLS
/// @param cls
/// @return
template <typename TCLS>
SInt32 RtlReleaseClass(TCLS* cls);

class __attribute__((uuid("d7c144b6-0792-44b8-b06b-02b227b547df"))) UnknownInterface
{
public:
	explicit UnknownInterface() = default;
	virtual ~UnknownInterface() = default;

	UnknownInterface& operator=(const UnknownInterface&) = default;
	UnknownInterface(const UnknownInterface&)			 = default;

	virtual SInt32			  Release()					   = 0;
	virtual void			  DecrementRef()			   = 0;
	virtual UnknownInterface* IncrementRef()			   = 0;
	virtual VoidPtr			  QueryInterface(UUID* p_uuid) = 0;
};

#endif

/// @note Part of NK loader API.

IMPORT_C VoidPtr RtlGetDLLProc(const char* symbol, VoidPtr dll_handle);

/// @brief Open DLL handle.
/// @param path 
/// @param drv 
/// @return 
IMPORT_C VoidPtr RtlOpenDLL(const char* path, const char* drv);

/// @brief Close DLL handle
/// @param dll_handle 
/// @return 
IMPORT_C UInt0 RtlCloseDLL(VoidPtr dll_handle);

/// @note Part of NK file API.

/// @brief Opens a file from a drive.
/// @param path the filesystem path.
/// @param drv drive name, use NULL to use default one.
/// @return the file descriptor of the file.
IMPORT_C UInt64 RtlOpenFile(const char* path, const char* drv);

/// @brief Closes a file and flushes its content.
/// @param file_desc the file descriptor.
/// @return 
IMPORT_C UInt0 RtlCloseFile(UInt64 file_desc);
