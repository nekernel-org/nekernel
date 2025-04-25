/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/**
  @brief The Explicit Partition Map scheme.
*/

#ifndef FIRMWAREKIT_EPM_H
#define FIRMWAREKIT_EPM_H

#include <NewKit/Defines.h>

#define kEPMNameLength (32)
#define kEPMFilesystemLength (16)
#define kEPMMagicLength (5)

/* @brief AMD64 magic for EPM */
#define kEPMMagic86 "EPMAM"

/* @brief RISC-V magic for EPM */
#define kEPMMagicRISCV "EPMRV"

/* @brief ARM magic for EPM */
#define kEPMMagicARM "EPMAR"

/* @brief 64x0 magic for EPM */
#define kEPMMagic64k "EPM64"

/* @brief 32x0 magic for EPM */
#define kEPMMagic32k "EPM32"

/* @brief POWER magic for EPM */
#define kEPMMagicPPC "EPMPC"

/* @brief Invalid magic for EPM */
#define kEPMMagicError "EPM??"

#ifdef __NE_AMD64__
#define kEPMMagic kEPMMagic86
#else
#ifdef __NE_ARM64__
#define kEPMMagic kEPMMagicARM
#else
#define kEPMMagic kEPMMagicError
#endif
#endif

///! @brief partition must start at this address.
///! Anything below is reserved for Data backup by the Main OS.
#define kEPMPartBlockLba (sizeof(EPM_PART_BLOCK))

///! @brief Current EPM revision.
#define kEPMRevisionBcd (0x0100)

/// !@brief EPM boot block address.
#define kEPMBootBlockLba (0U)

#define kEPMReserveLen (399)

struct EPM_GUID;
struct EPM_PART_BLOCK;

/* The first 0 > 128 addresses of a disk contains these headers. */

/// @brief EPM GUID structure.
typedef struct EPM_GUID {
  Kernel::UInt32 Data1;
  Kernel::UInt16 Data2;
  Kernel::UInt16 Data3;
  Kernel::UInt8  Data4[8];
} PACKED EPM_GUID;

/**
 * @brief The EPM boot block.
 * @note NumBlock and LbaStart are ignored on some platforms.
 */
struct PACKED EPM_PART_BLOCK {
  Kernel::Char  Magic[kEPMMagicLength] = {0};
  Kernel::Char  Name[kEPMNameLength]   = {0};
  EPM_GUID      Guid;
  Kernel::Int32 Version;
  Kernel::Int64 NumBlocks;
  Kernel::Int64 SectorSz;
  Kernel::Int64 LbaStart;  // base offset
  Kernel::Int64 LbaEnd;    // end offset
  Kernel::Int16 Kind;
  Kernel::Int16 Flags;
  Kernel::Int32 FsVersion;
  Kernel::Char  Fs[kEPMFilesystemLength];  /* NeFS, HeFS... */
  Kernel::Char  Reserved[kEPMReserveLen];  // to fill a full sector.
};

///! @brief Version kind enum.
///! @brief Use in boot block version field.

enum {
  kEPMInvalid   = 0x00,
  kEPMGeneric   = 0xcf,  /// @brief Generic OS
  kEPMLinux     = 0x8f,  /// @brief Linux on EPM
  kEPMBSD       = 0x9f,  /// @brief Berkeley Soft. Distribution
  kEPMNeKernel  = 0x1f,  /// @brief NeKernel.
  kEPMInvalidOS = 0xff,
};

inline EPM_GUID kEPMNilGuid = {0x0U, 0x0U, 0x0U, {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}};

#endif  // ifndef FIRMWAREKIT_EPM_H
