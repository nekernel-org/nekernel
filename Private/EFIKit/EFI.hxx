/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef __EFI__
#define __EFI__

/**
@brief HCore Implementation of UEFI protocols.
*/

#include <NewKit/Defines.hpp>

using namespace hCore;

typedef VoidPtr EfiHandlePtr;

struct EfiTableHeader;
struct EfiLoadFileProtocol;
struct EfiSimpleTextOutputProtocol;
struct EfiDevicePathProtocol;
struct EfiBootServices;

/// What's BootBolicy?
/// If TRUE, indicates that the request originates from the boot manager, and that the boot manager is attempting to
/// load FilePath as a boot selection. If FALSE, then FilePath must match an exact file to be loaded.

typedef UInt64 (*EfiTextString)(struct EfiSimpleTextOutputProtocol *This, const WideChar *OutputString);
typedef UInt64 (*EfiLoadFile)(EfiLoadFileProtocol *This, EfiDevicePathProtocol *FilePath, Boolean BootPolicy,
                              UInt32 *BufferSize, Void *Buffer);

/// EFI pool helpers

typedef enum EfiMemoryType
{
    ///
    /// Not used.
    ///
    EfiReservedMemoryType,
    ///
    /// The code portions of a loaded application.
    /// (Note that UEFI OS loaders are UEFI applications.)
    ///
    EfiLoaderCode,
    ///
    /// The data portions of a loaded application and the default data allocation
    /// type used by an application to allocate pool memory.
    ///
    EfiLoaderData,
    ///
    /// The code portions of a loaded Boot Services Driver.
    ///
    EfiBootServicesCode,
    ///
    /// The data portions of a loaded Boot Serves Driver, and the default data
    /// allocation type used by a Boot Services Driver to allocate pool memory.
    ///
    EfiBootServicesData,
    ///
    /// The code portions of a loaded Runtime Services Driver.
    ///
    EfiRuntimeServicesCode,
    ///
    /// The data portions of a loaded Runtime Services Driver and the default
    /// data allocation type used by a Runtime Services Driver to allocate pool memory.
    ///
    EfiRuntimeServicesData,
    ///
    /// Free (unallocated) memory.
    ///
    EfiConventionalMemory,
    ///
    /// Memory in which errors have been detected.
    ///
    EfiUnusableMemory,
    ///
    /// Memory that holds the ACPI tables.
    ///
    EfiACPIReclaimMemory,
    ///
    /// Address space reserved for use by the firmware.
    ///
    EfiACPIMemoryNVS,
    ///
    /// Used by system firmware to request that a memory-mapped IO region
    /// be mapped by the OS to a virtual address so it can be accessed by EFI runtime services.
    ///
    EfiMemoryMappedIO,
    ///
    /// System memory-mapped IO region that is used to translate memory
    /// cycles to IO cycles by the processor.
    ///
    EfiMemoryMappedIOPortSpace,
    ///
    /// Address space reserved by the firmware for code that is part of the processor.
    ///
    EfiPalCode,
    ///
    /// A memory region that operates as EfiConventionalMemory,
    /// however it happens to also support byte-addressable non-volatility.
    ///
    EfiPersistentMemory,
    ///
    /// A memory region that describes system memory that has not been accepted
    /// by a corresponding call to the underlying isolation architecture.
    ///
    EfiUnacceptedMemoryType,
    ///
    /// The last type of memory.
    /// Not a real type.
    ///
    EfiMaxMemoryType,
} EfiMemoryType;

typedef UInt64 (*EfiAllocatePool)(EfiMemoryType PoolType, UInt32 Size, VoidPtr *Buffer);
typedef UInt64 (*EfiFreePool)(VoidPtr Buffer);

typedef struct EfiTableHeader
{
    UInt64 Signature;
    UInt32 Revision;
    UInt32 HeaderSize;
    UInt32 Crc32;
    UInt32 Reserved;
} EfiTableHeader;

typedef struct EfiLoadFileProtocol
{
    EfiLoadFile LoadFile;
} EfiLoadFileProtocol;

typedef struct EfiDevicePathProtocol
{
    UInt8 Type;
    UInt8 SubType;
    UInt8 LengthData[2];
} EfiDevicePathProtocol;

typedef UInt64 (*EfiExitBootServices)(VoidPtr ImageHandle, UInt32 MapKey);

/**
@name EfiBootServices
@brief UEFI Boot Services record, it contains functions necessary to a firmware level application.
*/
typedef struct EfiBootServices
{
    EfiTableHeader SystemTable;
    UIntPtr RaiseTPL;
    UIntPtr RestoreTPL;
    UIntPtr AllocatePages;
    UIntPtr FreePages;
    UIntPtr GetMemoryMap;
    EfiAllocatePool AllocatePool;
    EfiFreePool FreePool;
    UIntPtr CreateEvent;
    UIntPtr SetTimer;
    UIntPtr WaitForEvent;
    UIntPtr SignalEvent;
    UIntPtr CloseEvent;
    UIntPtr CheckEvent;
    UIntPtr InstallProtocolInterface;
    UIntPtr ReinstallProtocolInterface;
    UIntPtr UninstallProtocolInterface;
    UIntPtr HandleProtocol;
    VoidPtr Reserved;
    UIntPtr RegisterProtocolNotify;
    UIntPtr LocateHandle;
    UIntPtr LocateDevicePath;
    UIntPtr InstallConfigurationTable;
    UIntPtr LoadImage;
    UIntPtr StartImage;
    UIntPtr Exit;
    UIntPtr UnloadImage;
    EfiExitBootServices ExitBootServices;
    UIntPtr GetNextMonotonicCount;
    UIntPtr Stall;
    UIntPtr SetWatchdogTimer;
    UIntPtr ConnectController;
    UIntPtr DisconnectController;
    UIntPtr OpenProtocol;
    UIntPtr CloseProtocol;
    UIntPtr OpenProtocolInformation;
    UIntPtr ProtocolsPerHandle;
    UIntPtr LocateHandleBuffer;
    UIntPtr LocateProtocol;
    UIntPtr InstallMultipleProtocolInterfaces;
    UIntPtr UninstallMultipleProtocolInterfaces;
    UIntPtr CalculateCrc32;
    UIntPtr CopyMem;
    UIntPtr SetMem;
    UIntPtr CreateEventEx;
} EfiBootServices;

#define kEntireDevPath 0xFF
#define kThisInstancePath 0x01

typedef struct EfiSimpleTextOutputProtocol
{
    UInt64 Reset;
    EfiTextString OutputString;
    UInt64 TestString;
    UInt64 QueryMode;
    UInt64 SetMode;
    UInt64 SetAttribute;
    UInt64 ClearScreen;
    UInt64 SetCursorPosition;
    UInt64 EnableCursor;
    UInt64 Mode;
} EfiSimpleTextOutputProtocol;

typedef struct EfiSystemTable
{
    EfiTableHeader SystemHeader;
    WideChar *FirmwareVendor;
    UInt32 FirmwareRevision;
    VoidPtr ConsoleInHandle;
    UInt64 ConIn;
    VoidPtr ConsoleOutHandle;
    EfiSimpleTextOutputProtocol *ConOut;
    VoidPtr StandardErrorHandle;
    UInt64 StdErr;
    UInt64 RuntimeServices;
    EfiBootServices *BootServices;
    UInt64 NumberOfTableEntries;
    UInt64 ConfigurationTable;
} EfiSystemTable;

#define EfiMain efi_main

#define kEfiOk 0
#define kEfiFail -1

#define EFI_EXTERN_C extern "C"

#endif // __EFI__
