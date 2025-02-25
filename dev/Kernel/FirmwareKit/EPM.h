/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

/**
	@brief The Explicit Partition Map scheme.
*/

#ifndef FIRMWAREKIT_EPM_H
#define FIRMWAREKIT_EPM_H

#include <NewKit/Defines.h>

#define kEPMNameLength		 (32)
#define kEPMFilesystemLength (16)
#define kEPMMagicLength		 (5)

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
typedef struct EPM_GUID
{
	NeOS::UInt32 Data1;
	NeOS::UInt16 Data2;
	NeOS::UInt16 Data3;
	NeOS::UInt8	 Data4[8];
} EPM_GUID;

/**
 * @brief The EPM boot block.
 * @note NumBlock and LbaStart are ignored on some platforms.
 */
struct PACKED EPM_PART_BLOCK
{
	NeOS::Char	Magic[kEPMMagicLength];
	NeOS::Char	Name[kEPMNameLength];
	EPM_GUID	Guid;
	NeOS::Int32 Version;
	NeOS::Int64 NumBlocks;
	NeOS::Int64 SectorSz;
	NeOS::Int64 LbaStart; // base offset
	NeOS::Int64 LbaEnd;	  // end offset
	NeOS::Int16 Kind;
	NeOS::Int16 Flags;
	NeOS::Int32 FsVersion;
	NeOS::Char	Fs[kEPMFilesystemLength]; /* NeFS, HPFS... */
	NeOS::Char	Reserved[kEPMReserveLen]; // to fill a full sector.
};

///! @brief Version kind enum.
///! @brief Use in boot block version field.

enum
{
	kEPMInvalid	  = 0x00,
	kEPMGenericOS = 0xcf, // Generic OS
	kEPMLinux	  = 0x8f, // Linux on EPM
	kEPMBSD		  = 0x9f, // Berkeley Soft. Distribution
	kEPMNeOS	  = 0x1f, // This OS.
	kEPMInvalidOS = 0xff,
};

#endif // ifndef FIRMWAREKIT_EPM_H
