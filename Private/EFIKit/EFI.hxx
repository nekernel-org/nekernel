/*
 *	========================================================
 *
 *	h-core
 * 	Copyright 2024, Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

/**
@brief HCore Implementation of UEFI protocols.
*/

#include <NewKit/Defines.hpp>

using namespace hCore;

struct EfiTableHeader;
struct EfiLoadFileProtocol;
struct EfiSimpleTextOutputProtocol;
struct EfiDevicePathProtocol;

/// What's boot policy?
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

typedef UInt64 (*EfiAllocatePool)(EfiMemoryType PoolType, UInt32 Size, Void **Buffer);
typedef UInt64 (*EfiFreePool)(Void *Buffer);

typedef struct EfiTableHeader
{
    UInt64 signature;
    UInt32 revision;
    UInt32 headerSize;
    UInt32 crc32;
    UInt32 reserved;
} EfiTableHeader;

typedef struct EfiLoadFileProtocol
{
    EfiLoadFile loadFile;
} EfiLoadFileProtocol;

typedef struct EfiDevicePathProtocol
{
    UInt8 type;
    UInt8 subType;
    UInt8 lenData[2];
} EfiDevicePathProtocol;

#define kEntireDevPath 0xFF
#define kThisInstancePath 0x01

typedef struct EfiSimpleTextOutputProtocol
{
    UInt64 reset;
    EfiTextString outputString;
    UInt64 testString;
    UInt64 queryMode;
    UInt64 setMode;
    UInt64 setAttribute;
    UInt64 clearScreen;
    UInt64 setCursorPosition;
    UInt64 enableCursor;
    UInt64 mode;
} EfiSimpleTextOutputProtocol;

typedef struct EfiSystemTable
{
    EfiTableHeader hdr;
    WideChar *firmwareVendor;
    UInt32 firmwareRevision;
    VoidPtr consoleInHandle;
    UInt64 conIn;
    VoidPtr consoleOutHandle;
    EfiSimpleTextOutputProtocol *conOut;
    VoidPtr standardErrorHandle;
    UInt64 stdErr;
    UInt64 runtimeServices;
    UInt64 bootServices;
    UInt64 numberOfTableEntries;
    UInt64 configurationTable;
} EfiSystemTable;

#define EfiMain efi_main

#define kEfiOk 0
