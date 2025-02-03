/* -------------------------------------------

Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

File: SCI.h
Purpose: System Calls.

------------------------------------------- */

#ifndef SCIKIT_FOUNDATION_H
#define SCIKIT_FOUNDATION_H

#include <Macros.h>

// ------------------------------------------------------------------------------------------ //
/// @brief Dynamic Loader API.
// ------------------------------------------------------------------------------------------ //

/// @brief Get function which is part of the DLL.
/// @param symbol the symbol to look for
/// @param dll_handle the DLL handle.
/// @return the proc pointer.
IMPORT_C SCIObject LdrGetDLLSymbolFromHandle(_Input const Char* symbol, _Input SCIObject dll_handle);

/// @brief Open DLL handle.
/// @param path
/// @param drv
/// @return
IMPORT_C SCIObject LdrOpenDLLHandle(_Input const Char* path, _Input const Char* drive_letter);

/// @brief Close DLL handle
/// @param dll_handle
/// @return
IMPORT_C Void LdrCloseDLLHandle(_Input SCIObject* dll_handle);

// ------------------------------------------------------------------------------------------ //
// File API.
// ------------------------------------------------------------------------------------------ //

/// @brief Opens a file from a drive.
/// @param fs_path the filesystem path.
/// @param drive_letter drive name, use NULL to use default drive location.
/// @return the file descriptor of the file.
IMPORT_C SCIObject IoOpenFile(const Char* fs_path, const Char* drive_letter);

/// @brief Closes a file and flushes its content.
/// @param file_desc the file descriptor.
/// @return Function doesn't return a type.
IMPORT_C Void IoCloseFile(_Input SCIObject file_desc);

/// @brief Write data to a file.
/// @param file_desc the file descriptor.
/// @param out_data the data to write.
/// @param sz_data the size of the data to write.
/// @return the number of bytes written.
IMPORT_C UInt32 IoWriteFile(_Input SCIObject file_desc, _Output VoidPtr out_data, SizeT sz_data);

/// @brief Read data from a file.
/// @param file_desc the file descriptor.
/// @param out_data the data to read.
/// @param sz_data the size of the data to read.
IMPORT_C UInt32 IoReadFile(_Input SCIObject file_desc, _Output VoidPtr* out_data, SizeT sz_data);

/// @brief Rewind the file pointer to the beginning of the file.
/// @param file_desc the file descriptor.
/// @return the number of bytes read.
IMPORT_C UInt64 IoRewindFile(_Input SCIObject file_desc);

/// @brief Tell the current position of the file pointer.
/// @param file_desc the file descriptor.
/// @return the current position of the file pointer.
IMPORT_C UInt64 IoTellFile(_Input SCIObject file_desc);

/// @brief Seek file offset from file descriptor.
IMPORT_C UInt64 IoSeekFile(_Input SCIObject file_desc, UInt64 file_offset);

// ------------------------------------------------------------------------
// Process API.
// ------------------------------------------------------------------------

/// @brief Spawns a Thread Information Block and Global Information Block inside the current process.
/// @param void.
/// @return > 0 error ocurred or already present, = 0 success.
IMPORT_C UInt32 RtlSpawnIB(Void);

/// @brief Spawns a process with a unique pid (stored as UIntPtr).
/// @param process_path process filesystem path.
/// @return > 0 process was created.
IMPORT_C UIntPtr RtlSpawnProcess(const Char* process_path, SizeT argc, Char** argv, Char** envp, SizeT envp_len);

/// @brief Exits a process with an exit_code.
/// @return if it has succeeded true, otherwise false.
IMPORT_C Bool RtlExitProcess(UIntPtr handle, UIntPtr exit_code);

/// @brief Get current PID of process.
/// @return Current process ID.
IMPORT_C UIntPtr RtlCurrentPID(Void);

// ------------------------------------------------------------------------
// Memory Manager API.
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

/// @brief Compare memory regions.
IMPORT_C SInt64 MmCmpMemory(_Input VoidPtr dest, _Input VoidPtr src, _Input SizeT len);

/// @brief Fill memory region.
IMPORT_C VoidPtr MmFillMemory(_Input VoidPtr dest, _Input SizeT len, _Input UInt8 value);

/// @brief Compare string regions.
IMPORT_C SInt64 MmStrCmp(_Input const Char* dest, _Input const Char* src);

/// @brief Get length of string.
IMPORT_C SInt64 MmStrLen(const Char* str);

// ------------------------------------------------------------------------
// Error API.
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

/// @brief Thread procedure function type.
typedef Void (*thread_proc_kind)(int argc, char** argv);

/// @brief Creates a thread.
/// @param procedure the thread procedure.
/// @param argument_count number of arguments inside that thread.
/// @param flags Thread flags.
/// @return the thread object.
IMPORT_C ThreadObject ThrCreateThread(thread_proc_kind procedure, SInt32 argument_count, SInt32 flags);

/// @brief Yields the current thread.
/// @param thread the thread to yield.
IMPORT_C Void ThrYieldThread(ThreadObject thrd);

/// @brief Joins a thread.
/// @param thread the thread to join.
IMPORT_C Void ThrJoinThread(ThreadObject thrd);

/// @brief Detach a thread.
/// @param thread the thread to detach.
IMPORT_C Void ThrDetachThread(ThreadObject thrd);

// ------------------------------------------------------------------------
// Drive Management API.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------ //
/// @brief Get the default drive letter.
/// @param void.
/// @return the drive letter.
// ------------------------------------------------------------------------------------------ //
IMPORT_C Char* DrvGetDefaultDriveLetter(Void);

// ------------------------------------------------------------------------------------------ //
/// @brief Get the drive letter from a path.
/// @param path the path.
/// @return the drive letter.
// ------------------------------------------------------------------------------------------ //
IMPORT_C Char* DrvGetDriveLetterFromPath(_Input const Char* path);

// ------------------------------------------------------------------------------------------ //
/// @brief Get a mounted drive from a letter.
/// @param letter the letter (A..Z).
/// @return the drive object.
// ------------------------------------------------------------------------------------------ //
IMPORT_C SCIObject DrvGetMountedDrive(_Input const Char letter);

// ------------------------------------------------------------------------------------------ //
/// @brief Mount a drive.
/// @param path the path to mount.
/// @param letter the letter to mount.
// ------------------------------------------------------------------------------------------ //
IMPORT_C Void DrvMountDrive(_Input const Char* path, _Input const Char* letter);

// ------------------------------------------------------------------------------------------ //
/// @brief Unmount a drive.
/// @param letter the letter to unmount.
// ------------------------------------------------------------------------------------------ //
IMPORT_C Void DrvUnmountDrive(_Input const Char letter);

// ------------------------------------------------------------------------
// Event handling API, use to listen to OS specific events.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------ //
/// @brief Add an event listener.
/// @param event_name the event name.
/// @param listener the listener to add.
/// @return the event listener.
// ------------------------------------------------------------------------------------------ //

IMPORT_C Void EvtAddListener(_Input const Char* event_name, _Input SCIObject listener);

// ------------------------------------------------------------------------------------------ //
/// @brief Remove an event listener.
/// @param event_name the event name.
/// @param listener the listener to remove.
/// @return the event listener.
// ------------------------------------------------------------------------------------------ //

IMPORT_C Void EvtRemoveListener(_Input const Char* event_name, _Input SCIObject listener);

// ------------------------------------------------------------------------------------------ //
/// @brief Dispatch an event.
/// @param event_name the event name.
/// @param event_data the event data.
/// @return the event data.
// ------------------------------------------------------------------------------------------ //

IMPORT_C VoidPtr EvtDispatchEvent(_Input const Char* event_name, _Input VoidPtr event_data);

// ------------------------------------------------------------------------------------------ //
// Power API.
// ------------------------------------------------------------------------------------------ //

enum
{
	kPowerCodeShutdown,
	kPowerCodeReboot,
	kPowerCodeSleep,
	kPowerCodeWake,
	kPowerCodeCount,
};

IMPORT_C SInt32 PwrReadCode(_Output SInt32& code);

IMPORT_C SInt32 PwrSendCode(_Output SInt32& code);

// ------------------------------------------------------------------------------------------ //
// CD-ROM API.
// ------------------------------------------------------------------------------------------ //

IMPORT_C SInt32 CdEjectDrive(_Input const Char drv_letter);

IMPORT_C SInt32 CdOpenTray(Void);

IMPORT_C SInt32 CdCloseTray(Void);

// ------------------------------------------------------------------------------------------ //
// Console API.
// ------------------------------------------------------------------------------------------ //

IMPORT_C SInt32 ConOut(IOObject file /* nullptr to direct to stdout */, const Char* fmt, ...);

IMPORT_C SInt32 ConIn(IOObject file /* nullptr to direct to stdout */, const Char* fmt, ...);

IMPORT_C IOObject ConCreate(Void);

IMPORT_C SInt32 ConRelease(IOObject);

IMPORT_C IOObject ConGet(const Char* path);

// ------------------------------------------------------------------------------------------ //
// Scheduler Interrupts API.
// ------------------------------------------------------------------------------------------ //

typedef SInt32 AffinityKind;
typedef UInt64 PID;

IMPORT_C SInt32 SchedAffinity(PID, SInt32 req, AffinityKind* local);

IMPORT_C SInt32 SchedTrace(PID, SInt32 req, VoidPtr address, VoidPtr data);

IMPORT_C SInt32 SchedKill(PID, SInt32 req);

#endif // ifndef SCIKIT_FOUNDATION_H
