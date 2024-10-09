/* -------------------------------------------

Copyright ZKA Technologies.

File: sci_base.hxx
Purpose: SCI core header file (C++ only).

------------------------------------------- */

#ifndef __SCI_BASE_HXX__
#define __SCI_BASE_HXX__

#include <sci/sci_hint.hxx>

#define ATTRIBUTE(X) __attribute__((X))

#define IMPORT_XPCOM extern "XPCOM"
#define IMPORT_CXX	 extern "C++"
#define IMPORT_C	 extern "C"

typedef bool Bool;
typedef void Void;

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

#include <sci/sci_lpc.hxx>

#ifdef __XPCOM_IMPL__
#include <sci/xpcom_core.hxx>
#else
class IUnknown; // Refrenced from an IDB entry.
class ICLSID;	// From the IDB, the constructor of the object, e.g: IAppCLSID.
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
/// @note Handle typedefs.
// ------------------------------------------------------------------------------------------ //

typedef VoidPtr Object;

typedef Object DLLObject;
typedef Object IOObject;
typedef Object SCMObject;
typedef Object ThreadObject;
typedef Object SocketObject;
typedef Object ShellObject;
typedef Object UIObject;

// ------------------------------------------------------------------------------------------ //

// ------------------------------------------------------------------------------------------ //
/// @note Dynamic Loader API.
// ------------------------------------------------------------------------------------------ //

/// @brief Get function which is part of the DLL.
/// @param symbol the symbol to look for
/// @param dll_handle the DLL handle.
/// @return the proc pointer.
IMPORT_C Object LdrGetDLLSymbolFromHandle(_Input const Char* symbol, _Input Object dll_handle);

/// @brief Open DLL handle.
/// @param path
/// @param drv
/// @return
IMPORT_C Object LdrOpenDLLHandle(_Input const Char* path, _Input const Char* drive_letter);

/// @brief Close DLL handle
/// @param dll_handle
/// @return
IMPORT_C Void LdrCloseDLLHandle(_Input Object* dll_handle);

// ------------------------------------------------------------------------------------------ //
// File API.
// ------------------------------------------------------------------------------------------ //

/// @brief Opens a file from a drive.
/// @param fs_path the filesystem path.
/// @param drive_letter drive name, use NULL to use default one.
/// @return the file descriptor of the file.
IMPORT_C Object IoOpenFile(const Char* fs_path, const Char* drive_letter);

/// @brief Closes a file and flushes its content.
/// @param file_desc the file descriptor.
/// @return void.
IMPORT_C Void IoCloseFile(_Input Object file_desc);

/// @brief Write data to a file.
/// @param file_desc the file descriptor.
/// @param out_data the data to write.
/// @param sz_data the size of the data to write.
/// @return the number of bytes written.
IMPORT_C UInt32 IoWriteFile(_Input Object file_desc, _Output VoidPtr out_data, SizeT sz_data);

/// @brief Read data from a file.
/// @param file_desc the file descriptor.
/// @param out_data the data to read.
/// @param sz_data the size of the data to read.
IMPORT_C UInt32 IoReadFile(_Input Object file_desc, _Output VoidPtr* out_data, SizeT sz_data);

/// @brief Rewind the file pointer to the beginning of the file.
/// @param file_desc the file descriptor.
/// @return the number of bytes read.
IMPORT_C UInt64 IoRewindFile(_Input Object file_desc);

/// @brief Tell the current position of the file pointer.
/// @param file_desc the file descriptor.
/// @return the current position of the file pointer.
IMPORT_C UInt64 IoTellFile(_Input Object file_desc);

IMPORT_C UInt64 IoSeekFile(_Input Object file_desc, UInt64 file_offset);

// ------------------------------------------------------------------------
// TLS API.
// ------------------------------------------------------------------------

/// @brief Installs the Thread Information Block and Global Information Block inside the current process.
/// @param void.
/// @return > 0 error ocurred or already present, = 0 success.
IMPORT_C UInt32 RtlTlsInstall(Void);

#ifndef __XPCOM_IMPL__

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
IMPORT_C SInt32 XPCOMCreateInstance(_Input UInt32 flags, _Output Object* handle_instance);

/// @brief Destroys an XPCOM instance of the process.
/// @param handle_instance the XPCOM handle.
IMPORT_C Void XPCOMDestroyInstance(_Input Object handle_instance);

#endif // !__XPCOM_IMPL__

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
IMPORT_C Void MmDestroyHeap(_Input VoidPtr heap);

/// @brief Change protection flags of a memory region.
IMPORT_C Void MmSetHeapFlags(_Input VoidPtr heap, _Input UInt32 flags);

/// @brief Change protection flags of a memory region.
IMPORT_C UInt32 MmGetHeapFlags(_Input VoidPtr heap);

/// @brief Fill memory region with CRC32.	
IMPORT_C UInt32 MmFillCRC32Heap(_Input VoidPtr heap);

/// @brief Copy memory region.
IMPORT_C VoidPtr MmCopyMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len);

/// @brief Fill memory region.
IMPORT_C VoidPtr MmFillMemory(_Input VoidPtr dest, _Input SizeT len, _Input UInt8 value);

// ------------------------------------------------------------------------
// Error handling API.
// ------------------------------------------------------------------------

IMPORT_C SInt32 ErrGetLastError(Void);

// ------------------------------------------------------------------------
// Threading API.
// ------------------------------------------------------------------------

/// @brief Exit the current thread.
/// @param exit_code the exit code.
IMPORT_C Void ThrExitCurrentThread(_Input SInt32 exit_code);

/// @brief Exit the main thread.
/// @param exit_code the exit code.
IMPORT_C Void ThrExitMainThread(_Input SInt32 exit_code);

/// @brief Exit a thread.
/// @param thread the thread to exit.
/// @param exit_code the exit code.
IMPORT_C Void ThrExitThread(_Input ThreadObject thread, _Input SInt32 exit_code);

typedef Void(*ThreadProc)(Void);

/// @brief Create a thread.
/// @param procedure the thread procedure.
/// @param argument_count number of arguments inside that thread.
/// @param flags Thread flags.
/// @return the thread object.
IMPORT_C ThreadObject ThrCreateThread(ThreadProc procedure, SInt32 argument_count, SInt32 flags);

/// @brief Yield the current thread.
/// @param thread the thread to yield.
IMPORT_C Void ThrExitYieldThread(Void);

/// @brief Join a thread.
/// @param thread the thread to join.
IMPORT_C Void ThrExitJoinThread(Void);

/// @brief Detach a thread.
/// @param thread the thread to detach.
IMPORT_C Void ThrExitDetachThread(Void);

// ------------------------------------------------------------------------
// Drive Management API.
// ------------------------------------------------------------------------

/// @brief Get the default drive letter.
/// @param void.
/// @return the drive letter.
IMPORT_C Char* DrvGetDefaultDriveLetter(Void);

/// @brief Get the drive letter from a path.
/// @param path the path.
/// @return the drive letter.
IMPORT_C Char* DrvGetDriveLetterFromPath(_Input const Char* path);

/// @brief Get a mounted drive from a letter.
/// @param letter the letter (A..Z).
/// @return the drive object.
IMPORT_C Object DrvGetMountedDrive(_Input const Char letter);

/// @brief Mount a drive.
/// @param path the path to mount.
/// @param letter the letter to mount.
IMPORT_C Void DrvMountDrive(_Input const Char* path, _Input const Char* letter);

/// @brief Unmount a drive.
/// @param letter the letter to unmount.
IMPORT_C Void DrvUnmountDrive(_Input const Char letter);

// ------------------------------------------------------------------------
// Event handling API, use to listen to OS specific events.
// ------------------------------------------------------------------------

/// @brief Add an event listener.
/// @param event_name the event name.
/// @param listener the listener to add.
/// @return the event listener.
IMPORT_C Void EvtAddListener(_Input const Char* event_name, _Input Object listener);

/// @brief Remove an event listener.
/// @param event_name the event name.
/// @param listener the listener to remove.
/// @return the event listener.
IMPORT_C Void EvtRemoveListener(_Input const Char* event_name, _Input Object listener);

/// @brief Dispatch an event.
/// @param event_name the event name.
/// @param event_data the event data.
/// @return the event data.
IMPORT_C VoidPtr EvtDispatchEvent(_Input const Char* event_name, _Input VoidPtr event_data);

#endif // ifndef __SCI_BASE_HXX__
