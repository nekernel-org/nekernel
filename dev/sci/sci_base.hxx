/* -------------------------------------------

Copyright ZKA Technologies.

File: SCIBase.hxx
Purpose: sci/M core header file (C++)

------------------------------------------- */

#ifndef __SCI_BASE_HXX__
#define __SCI_BASE_HXX__

#include <sci/sci_hint.hxx>

#define ATTRIBUTE(X) __attribute__((X))

#define IMPORT_CXX extern "C++"
#define IMPORT_C   extern "C"

typedef bool Bool;
typedef void UInt0;

typedef UInt0 Void;

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

#include <sci/sci_err.hxx>

#ifdef __SCI_IMPL__
#include <sci/xpcom_core.hxx>
#else
class IUnknown; // Refrenced from an IDB entry.
class ICLSID;	// From the IDB, the constructor of the object, e.g: WordUCLSID.
class UUID;

class ATTRIBUTE(uuid("d7c144b6-0792-44b8-b06b-02b227b547df")) IUnknown
{
public:
	explicit IUnknown() = default;
	virtual ~IUnknown() = default;

	IUnknown& operator=(const IUnknown&) = default;
	IUnknown(const IUnknown&)			 = default;

	virtual SInt32	  Release()				   = 0;
	virtual void	  RemoveRef()			   = 0;
	virtual IUnknown* AddRef()				   = 0;
	virtual VoidPtr	  QueryClass(UUID* p_uuid) = 0;
};

template <typename FnSign, typename ClsID>
class IEventListener : public ClsID
{
	friend ClsID;

	explicit IEventListener() = default;
	virtual ~IEventListener() = default;

	IEventListener& operator=(const IEventListener&) = default;
	IEventListener(const IEventListener&)			 = default;

	virtual IEventListener& operator-=(const Char* event_name);
	virtual IEventListener& operator+=(FnSign arg) = 0;
};
#endif

// ------------------------------------------------------------------------------------------ //
/// @note Handle types.
// ------------------------------------------------------------------------------------------ //

typedef VoidPtr ZKAObject;

typedef ZKAObject ZKADLLObject;
typedef ZKAObject ZKAIOObject;
typedef ZKAObject ZKASCMObject;
typedef ZKAObject ZKAThreadObject;
typedef ZKAObject ZKASocketObject;

// ------------------------------------------------------------------------------------------ //

// ------------------------------------------------------------------------------------------ //
/// @note Part of ZKA loader API.
// ------------------------------------------------------------------------------------------ //

/// @brief Get function which is part of the DLL.
/// @param symbol the symbol to look for
/// @param dll_handle the DLL handle.
/// @return the proc pointer.
IMPORT_C ZKAObject LdrGetDLLProc(_Input const Char* symbol, _Input ZKAObject dll_handle);

/// @brief Open DLL handle.
/// @param path
/// @param drv
/// @return
IMPORT_C ZKAObject LdrOpenDLL(_Input const Char* path, _Input const Char* drive_letter);

/// @brief Close DLL handle
/// @param dll_handle
/// @return
IMPORT_C UInt0 LdrCloseDLL(_Input ZKAObject dll_handle);

// ------------------------------------------------------------------------------------------ //
// File API.
// ------------------------------------------------------------------------------------------ //

/// @brief Opens a file from a drive.
/// @param fs_path the filesystem path.
/// @param drive_letter drive name, use NULL to use default one.
/// @return the file descriptor of the file.
IMPORT_C ZKAObject IoOpenFile(const Char* fs_path, const Char* drive_letter);

/// @brief Closes a file and flushes its content.
/// @param file_desc the file descriptor.
/// @return void.
IMPORT_C UInt0 IoCloseFile(_Input ZKAObject file_desc);

IMPORT_C UInt32 IoWriteFile(_Input ZKAObject file_desc, _Output VoidPtr out_data, SizeT sz_data);

IMPORT_C UInt32 IoReadFile(_Input ZKAObject file_desc, _Output VoidPtr* out_data, SizeT sz_data);

IMPORT_C UInt64 IoRewindFile(_Input ZKAObject file_desc);

IMPORT_C UInt64 IoTellFile(_Input ZKAObject file_desc);

IMPORT_C UInt64 IoSeekFile(_Input ZKAObject file_desc, UInt64 file_offset);

// ------------------------------------------------------------------------
// TLS API.
// ------------------------------------------------------------------------

/// @brief Installs the Thread Information Block and Global Information Block inside the current process.
/// @param void.
/// @return > 0 error ocurred or already present, = 0 success.
IMPORT_C UInt32 RtlTlsInstall(Void);

#ifndef __SCI_IMPL__

// ------------------------------------------------------------------------
// XPCOM API.
// ------------------------------------------------------------------------

/// @brief Allocate new XPCOM object.
/// @tparam TCLS the class type.
/// @tparam UCLSID UCLS factory class type.
/// @param uclsidOfCls UCLS factory class
/// @return TCLS interface
template <typename TCLS, typename UCLSID, typename... Args>
TCLS* XPCOMQueryClass(_Input UCLSID* uclsidOfCls, _Input Args&&... args);

/// @brief Release XPCOM object.
/// @tparam TCLS the class type.
/// @param cls the class to release.
/// @return status code.
template <typename TCLS>
SInt32 XPCOMReleaseClass(_Input TCLS* cls);

/// @brief Creates an XPCOM instance in the process.
/// @param handle_instance the XPCOM handle.
/// @param flags the XPCOM flags.
IMPORT_C SInt32 XPCOMCreateInstance(_Input UInt32 flags, _Output ZKAObject* handle_instance);

/// @brief Destroys an XPCOM instance of the process.
/// @param handle_instance the XPCOM handle.
IMPORT_C UInt0 XPCOMDestroyInstance(_Input ZKAObject handle_instance);

#endif // !__SCI_IMPL__

// ------------------------------------------------------------------------
// Memory Management API.
// ------------------------------------------------------------------------

/// @brief Creates a new heap from the process's address space.
/// @param len the length of it.
/// @param flags the flags of it.
/// @return heap pointer.
IMPORT_C VoidPtr MmCreateHeap(_Input SizeT len, _Input UInt32 flags);

/// @brief Destroys the pointer
/// @param heap the heap itself.
/// @return void.
IMPORT_C UInt0 MmDestroyHeap(_Input VoidPtr heap);

/// @brief Change protection flags of memory region.
IMPORT_C UInt32 MmChangeHeapFlags(_Input VoidPtr heap, _Input UInt32 flags);

/// @brief Fill memory region with CRC32.
IMPORT_C UInt32 MmFillCRC32Heap(_Input VoidPtr heap);

// ------------------------------------------------------------------------
// Error handling API.
// ------------------------------------------------------------------------

IMPORT_C SInt32 ErrGetLastError(UInt0);

// ------------------------------------------------------------------------
// Threading API.
// ------------------------------------------------------------------------

IMPORT_C UInt0 ThrExitCurrentThread(_Input SInt32 exit_code);
IMPORT_C UInt0 ThrExitMainThread(_Input SInt32 exit_code);

IMPORT_C UInt0 ThrExitYieldThread(UInt0);

IMPORT_C UInt0 ThrExitJoinThread(UInt0);
IMPORT_C UInt0 ThrExitDetachThread(UInt0);

// ------------------------------------------------------------------------
// Drive Management API.
// ------------------------------------------------------------------------

#endif // ifndef __SCI_BASE_HXX__
