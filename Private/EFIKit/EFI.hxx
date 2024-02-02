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
@brief HCore Implementation of EFI.
@author Amlal El Mahrouss
*/

#include <NewKit/Defines.hpp>

using namespace HCore;

#ifndef EPI_API
#define EFI_API __attribute__((stdcall))
#endif  // ifndef EPI_API

// Forwar decls

struct EfiTableHeader;
struct EfiLoadFileProtocol;
struct EfiSimpleTextOutputProtocol;
struct EfiDevicePathProtocol;
struct EfiBootServices;
struct EfiMemoryDescriptor;
struct EfiSystemTable;
struct EfiGUID;
struct EfiFileDevicePathProtocol;
struct EfiHandle;

/// @brief Core Handle Type
/// This is like NT's Win32 HANDLE type.
typedef struct EfiHandle {
} *EfiHandlePtr;

/* UEFI uses wide characters by default. */
typedef WideChar EfiCharType;

/// What's BootBolicy?
/// If TRUE, indicates that the request originates from the boot manager, and
/// that the boot manager is attempting to load FilePath as a boot selection. If
/// FALSE, then FilePath must match an exact file to be loaded.

typedef UInt64(EFI_API *EfiTextString)(struct EfiSimpleTextOutputProtocol *This,
                                       const WideChar *OutputString);

typedef UInt64(EFI_API *EfiTextAttrib)(struct EfiSimpleTextOutputProtocol *This,
                                       const WideChar Attribute);

typedef UInt64(EFI_API *EfiTextClear)(struct EfiSimpleTextOutputProtocol *This);

typedef UInt64(EFI_API *EfiLoadFile)(EfiLoadFileProtocol *This,
                                     EfiFileDevicePathProtocol *FilePath,
                                     Boolean BootPolicy, UInt32 *BufferSize,
                                     VoidPtr Buffer);

typedef UInt64(EFI_API *EfiCopyMem)(VoidPtr DstBuf, VoidPtr SrcBuf,
                                    SizeT Length);
typedef UInt64(EFI_API *EfiSetMem)(VoidPtr DstBuf, Char Byte, SizeT Length);

typedef UInt64(EFI_API *EfiLocateDevicePath)(EfiGUID *Protocol,
                                             EfiDevicePathProtocol **DevicePath,
                                             EfiHandlePtr Device);

/// EFI pool helpers, taken from iPXE.

typedef enum EfiMemoryType {
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
  /// data allocation type used by a Runtime Services Driver to allocate pool
  /// memory.
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
  /// be mapped by the OS to a virtual address so it can be accessed by EFI
  /// runtime services.
  ///
  EfiMemoryMappedIO,
  ///
  /// System memory-mapped IO region that is used to translate memory
  /// cycles to IO cycles by the processor.
  ///
  EfiMemoryMappedIOPortSpace,
  ///
  /// Address space reserved by the firmware for code that is part of the
  /// processor.
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

typedef enum EfiAllocateType {
  /// Anything that satisfy the request.
  AllocateAnyPages,
  AllocateMaxAddress,
  ///
  /// Allocate pages at a specified address.
  ///
  AllocateAddress,
  ///
  /// Maximum enumeration value that may be used for bounds checking.
  ///
  MaxAllocateType
} EfiAllocateType;

typedef struct EfiMemoryDescriptor {
  ///
  /// Type of the memory region.
  /// Type EFI_MEMORY_TYPE is defined in the
  /// AllocatePages() function description.
  ///
  UInt32 Type;
  ///
  /// Physical address of the first byte in the memory region. PhysicalStart
  /// must be aligned on a 4 KiB boundary, and must not be above
  /// 0xfffffffffffff000. Type EFI_PHYSICAL_ADDRESS is defined in the
  /// AllocatePages() function description
  ///
  UIntPtr PhysicalStart;
  ///
  /// Virtual address of the first byte in the memory region.
  /// VirtualStart must be aligned on a 4 KiB boundary,
  /// and must not be above 0xfffffffffffff000.
  ///
  UIntPtr VirtualStart;
  ///
  /// NumberOfPagesNumber of 4 KiB pages in the memory region.
  /// NumberOfPages must not be 0, and must not be any value
  /// that would represent a memory page with a start address,
  /// either physical or virtual, above 0xfffffffffffff000.
  ///
  UInt64 NumberOfPages;
  ///
  /// Attributes of the memory region that describe the bit mask of capabilities
  /// for that memory region, and not necessarily the current settings for that
  /// memory region.
  ///
  UInt64 Attribute;
} EfiMemoryDescriptor;

typedef UInt64(EFI_API *EfiAllocatePool)(EfiMemoryType PoolType, UInt32 Size,
                                         VoidPtr *Buffer);

typedef UInt64(EFI_API *EfiFreePool)(VoidPtr Buffer);

typedef UInt64(EFI_API *EfiCalculateCrc32)(VoidPtr Data, UInt32 DataSize,
                                           UInt32 *CrcOut);

/**
@brief Present in every header, used to identify a UEFI structure.
*/
typedef struct EfiTableHeader {
  UInt64 Signature;
  UInt32 Revision;
  UInt32 HeaderSize;
  UInt32 Crc32;
  UInt32 Reserved;
} EfiTableHeader;

#define EFI_LOAD_FILE_PROTOCOL_GUID                  \
  {                                                  \
    0x56EC3091, 0x954C, 0x11d2, {                    \
      0x8e, 0x3f, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b \
    }                                                \
  }

#define EFI_LOAD_FILE2_PROTOCOL_GUID                 \
  {                                                  \
    0x4006c0c1, 0xfcb3, 0x403e, {                    \
      0x99, 0x6d, 0x4a, 0x6c, 0x87, 0x24, 0xe0, 0x6d \
    }                                                \
  }

#define EFI_LOADED_IMAGE_PROTOCOL_GUID               \
  {                                                  \
    0x5B1B31A1, 0x9562, 0x11d2, {                    \
      0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B \
    }                                                \
  }

#define EFI_LOADED_IMAGE_PROTOCOL_REVISION 0x1000

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID         \
  {                                                  \
    0x0964e5b22, 0x6459, 0x11d2, {                   \
      0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b \
    }                                                \
  }

#define EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID   \
  {                                                  \
    0xbc62157e, 0x3e33, 0x4fec, {                    \
      0x99, 0x20, 0x2d, 0x3b, 0x36, 0xd7, 0x50, 0xdf \
    }                                                \
  }

typedef UInt64(EfiImageUnload)(EfiHandlePtr ImageHandle);

typedef struct EfiLoadImageProtocol {
  UInt32 Revision;
  EfiHandlePtr ParentHandle;
  EfiSystemTable *SystemTable;

  // Source location of the image
  EfiHandlePtr DeviceHandle;
  EfiDevicePathProtocol *FilePath;
  Void *Reserved;

  // Image’s load options
  UInt32 LoadOptionsSize;
  Void *LoadOptions;

  // Location where image was loaded
  Void *ImageBase;
  UInt64 ImageSize;
  EfiMemoryType ImageCodeType;
  EfiMemoryType ImageDataType;
  EfiImageUnload Unload;
} EfiLoadImageProtocol;

typedef struct EfiLoadFileProtocol {
  EfiLoadFile LoadFile;
} EfiLoadFileProtocol;

typedef struct EfiDevicePathProtocol {
  UInt8 Type;
  UInt8 SubType;
  UInt8 Length[2];
} EfiDevicePathProtocol;

typedef struct EfiFileDevicePathProtocol {
  EfiDevicePathProtocol Proto;

  ///
  ///   File Path of this struct
  ///
  WideChar Path[kPathLen];
} EfiFileDevicePathProtocol;

typedef UInt64 EfiPhysicalAddress;
typedef UIntPtr EfiVirtualAddress;

typedef UInt64(EFI_API *EfiExitBootServices)(VoidPtr ImageHandle,
                                             UInt32 MapKey);

typedef UInt64(EFI_API *EfiAllocatePages)(EfiAllocateType AllocType,
                                          EfiMemoryType MemType,
                                          EfiPhysicalAddress *Memory);

typedef UInt64(EFI_API *EfiFreePages)(EfiPhysicalAddress *Memory, UInt32 Pages);

/**
 * @brief GUID type, something you can also find in CFKit.
 */
typedef struct EfiGUID {
  UInt32 Data1;
  UInt16 Data2;
  UInt16 Data3;
  UInt8 Data4[8];
} EfiGUID;

/***
 * Protocol stuff...
 */

/** some helpers */
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020

typedef UInt64(EFI_API *EfiLocateProtocol)(EfiGUID *Protocol,
                                           VoidPtr Registration,
                                           VoidPtr *Interface);

typedef UInt64(EFI_API *EfiOpenProtocol)(EfiHandlePtr Handle, EfiGUID *Guid,
                                         VoidPtr *Interface,
                                         EfiHandlePtr AgentHandle,
                                         EfiHandlePtr ControllerHandle,
                                         UInt32 Attributes);

/**
@name EfiBootServices
@brief UEFI Boot Services record, it contains functions necessary to a
firmware level application.
*/
typedef struct EfiBootServices {
  EfiTableHeader SystemTable;
  UIntPtr RaiseTPL;
  UIntPtr RestoreTPL;
  EfiAllocatePages AllocatePages;
  EfiFreePages FreePages;
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
  EfiLocateDevicePath LocateDevicePath;
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
  EfiOpenProtocol OpenProtocol;
  UIntPtr CloseProtocol;
  UIntPtr OpenProtocolInformation;
  UIntPtr ProtocolsPerHandle;
  UIntPtr LocateHandleBuffer;
  EfiLocateProtocol LocateProtocol;
  UIntPtr InstallMultipleProtocolInterfaces;
  UIntPtr UninstallMultipleProtocolInterfaces;
  EfiCalculateCrc32 CalculateCrc32;
  EfiCopyMem CopyMem;
  EfiSetMem SetMem;
  UIntPtr CreateEventEx;
} EfiBootServices;

#define kEntireDevPath 0xFF
#define kThisInstancePath 0x01

/**
@brief PrintF like protocol.
*/
typedef struct EfiSimpleTextOutputProtocol {
  VoidPtr Reset;
  EfiTextString OutputString;
  VoidPtr TestString;
  VoidPtr QueryMode;
  VoidPtr SetMode;
  EfiTextAttrib SetAttribute;
  EfiTextClear ClearScreen;
  VoidPtr SetCursorPosition;
  VoidPtr EnableCursor;
  VoidPtr Mode;
} EfiSimpleTextOutputProtocol;

/**
@brief The Structure that they give you when booting.
*/
typedef struct EfiSystemTable {
  EfiTableHeader SystemHeader;
  WideChar *FirmwareVendor;
  UInt32 FirmwareRevision;
  EfiHandlePtr ConsoleInHandle;
  VoidPtr ConIn;
  EfiHandlePtr ConsoleOutHandle;
  EfiSimpleTextOutputProtocol *ConOut;
  EfiHandlePtr StandardErrorHandle;
  VoidPtr StdErr;
  VoidPtr RuntimeServices;
  EfiBootServices *BootServices;
  UInt64 NumberOfTableEntries;
  VoidPtr ConfigurationTable;
} EfiSystemTable;

#define EfiMain efi_main

#define kEfiOk 0
#define kEfiFail -1

#define EFI_EXTERN_C extern "C"

typedef struct EfiIPV4 {
  UInt8 Addr[4];
} EfiIPV4;

///
/// 16-byte buffer. An IPv6 internet protocol address.
///
typedef struct EfiIPV6 {
  UInt8 Addr[16];
} EfiIPV6;

#define kEFIYellow (0x01 | 0x02 | 0x04 | 0x08)

#ifdef __x86_64
#define __EFI_x86_64__ 1
#endif  // __x86_64

enum {
  kEFIHwDevicePath = 0x01,
  kEFIAcpiDevicePath = 0x02,
  kEFIMessaingDevicePath = 0x03,
  kEFIMediaDevicePath = 0x04,
  kEFIBiosBootPath = 0x05,
  kEFIEndOfPath = 0x06,
  kEFICount = 6,
};

#define END_DEVICE_PATH_TYPE 0x7f
#define END_ENTIRE_DEVICE_PATH_SUBTYPE 0xFF
#define END_INSTANCE_DEVICE_PATH_SUBTYPE 0x01

#define kEfiOffsetOf(T, F) __builtin_offsetof(T, F)

#endif  // __EFI__
