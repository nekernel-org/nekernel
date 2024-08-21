/* -------------------------------------------

Copyright ZKA Technologies.

File: SCIBase.hxx
Purpose: SCI/M core header file (C++)

------------------------------------------- */

#ifndef __SCI_BASE_HXX__
#define __SCI_BASE_HXX__

#include <SCI/Hint.h>

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

#ifdef __SCI_IMPL__
#include <SCI/scm.internal.inl>
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
	virtual void			  RemoveRef()			   = 0;
	virtual UnknownInterface* AddRef()			   = 0;
	virtual VoidPtr			  QueryInterface(UUID* p_uuid) = 0;
};

template <typename FnSign, typename ClsID>
class EventListenerInterface : public ClsID
{
public:
	explicit EventListenerInterface() = default;
	virtual ~EventListenerInterface() = default;

	EventListenerInterface& operator=(const EventListenerInterface&) = default;
	EventListenerInterface(const EventListenerInterface&)			 = default;

    virtual EventListenerInterface& operator +=(FnSign arg) = 0;
};
#endif

// ------------------------------------------------------------------------------------------ //
/// @note Handle types.
// ------------------------------------------------------------------------------------------ //

typedef VoidPtr NEW_OBJECT;

typedef NEW_OBJECT DLL_OBJECT;
typedef NEW_OBJECT IO_OBJECT;
typedef NEW_OBJECT COMP_OBJECT;
typedef NEW_OBJECT SCM_OBJECT;

// ------------------------------------------------------------------------------------------ //

// ------------------------------------------------------------------------------------------ //
/// @note Part of ZKA loader API.
// ------------------------------------------------------------------------------------------ //

/// @brief Get function which is part of the DLL.
/// @param symbol the symbol to look for
/// @param dll_handle the DLL handle.
/// @return the proc pointer.
IMPORT_C NEW_OBJECT LdrGetDLLProc(_Input const Char* symbol, _Input NEW_OBJECT dll_handle);

/// @brief Open DLL handle.
/// @param path
/// @param drv
/// @return
IMPORT_C NEW_OBJECT LdrOpenDLL(_Input const Char* path, _Input const Char* drive_letter);

/// @brief Close DLL handle
/// @param dll_handle
/// @return
IMPORT_C UInt0 LdrCloseDLL(_Input NEW_OBJECT dll_handle);

// ------------------------------------------------------------------------------------------ //
// File API.
// ------------------------------------------------------------------------------------------ //

/// @brief Opens a file from a drive.
/// @param fs_path the filesystem path.
/// @param drive_letter drive name, use NULL to use default one.
/// @return the file descriptor of the file.
IMPORT_C NEW_OBJECT RtlOpenFile(const Char* fs_path, const Char* drive_letter);

/// @brief Closes a file and flushes its content.
/// @param file_desc the file descriptor.
/// @return
IMPORT_C UInt0 RtlCloseFile(_Input NEW_OBJECT file_desc);

// ------------------------------------------------------------------------
// TLS API.
// ------------------------------------------------------------------------

/// @brief Installs the TIB and GIB inside the current process.
/// @param void.
/// @return > 0 error ocurred or already present, = 0 success.
IMPORT_C UInt32 TlsInstallIB(UInt0);

// ------------------------------------------------------------------------
// SCM API.
// ------------------------------------------------------------------------

/// @brief Allocate new SCM object.
/// @tparam TCLS the class type.
/// @tparam UCLSID UCLS factory class type.
/// @param uclsidOfCls UCLS factory class
/// @return TCLS interface
template <typename TCLS, typename UCLSID, typename... Args>
TCLS* ScmQueryInterface(_Input UCLSID* uclsidOfCls, _Input Args&&... args);

/// @brief Release SCM object.
/// @tparam TCLS the class type.
/// @param cls the class to release.
/// @return status code.
template <typename TCLS>
SInt32 ScmReleaseClass(_Input TCLS* cls);

/// @brief Creates an SCM instance in the process.
/// @param handle_instance the SCM handle.
/// @param flags the SCM flags.
IMPORT_C SInt32 ScmCreateInstance(_Input UInt32 flags, _Output SCM_OBJECT* handle_instance);

/// @brief Destroys an SCM instance of the process.
/// @param handle_instance the SCM handle.
IMPORT_C UInt0  ScmDestroyInstance(_Input SCM_OBJECT handle_instance);

// ------------------------------------------------------------------------
// Memory Management API.
// ------------------------------------------------------------------------

/// @brief Creates a new heap from the process's address space.
/// @param len the length of it.
/// @param flags the flags of it.
/// @return heap pointer.
IMPORT_C VoidPtr RtlCreateHeap(_Input SizeT len, _Input UInt32 flags);

/// @brief Destroys the pointer
/// @param heap the heap itself.
/// @return void.
IMPORT_C UInt0 RtlDestroyHeap(_Input VoidPtr heap);

// ------------------------------------------------------------------------
// Error handling API.
// ------------------------------------------------------------------------

IMPORT_C SInt32 ErrGetLastError(UInt0);

// ------------------------------------------------------------------------
// Threading API.
// ------------------------------------------------------------------------

IMPORT_C UInt0 ThrExitCurrentThread(SInt32 exit_code);
IMPORT_C UInt0 ThrExitMainThread(SInt32 exit_code);

IMPORT_C UInt0 ThrExitYieldThread(UInt0);

IMPORT_C UInt0 ThrExitJoinThread(UInt0);
IMPORT_C UInt0 ThrExitDetachThread(UInt0);

#endif // ifndef __SCI_BASE_HXX__
