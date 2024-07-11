/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

/**
	@brief The Explicit Partition Map scheme.
*/

#ifndef __FIRMWARE_EPM_HXX__
#define __FIRMWARE_EPM_HXX__

#include <NewKit/Defines.hpp>

#define kEPMNameLength		 (32)
#define kEPMFilesystemLength (16)
#define kEPMMagicLength		 (5)

/* The first 0 > 128 addresses of a disk contains these headers. */

/// @brief EPM GUID structure.
typedef struct BlockGUID
{
	Kernel::UInt32 Data1;
	Kernel::UInt16 Data2;
	Kernel::UInt16 Data3;
	Kernel::UInt8  Data4[8];
} BlockGUID;

/**
 * @brief The EPM boot block.
 * @note NumBlock and LbaStart are ignored on some platforms.
 */
struct PACKED BootBlock
{
	Kernel::Char  Magic[kEPMMagicLength];
	Kernel::Char  Name[kEPMNameLength];
	BlockGUID	  Uuid;
	Kernel::Int32 Version;
	Kernel::Int64 NumBlocks;
	Kernel::Int64 SectorSz;
	Kernel::Int64 LbaStart;
	Kernel::Int16 Kind;
	Kernel::Int32 FsVersion;
	Kernel::Char  Fs[kEPMFilesystemLength]; /* NewFS, ffs2... */
	Kernel::Char  Reserved[409];			// to fill a full sector.
};

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

/* @brief UEFI magic for EPM */

#define kEPMMagicUEFI "EPMUE"

/* @brief CoreBoot magic for EPM */

#define kEPMMagicCoreBoot "EPMCB"

/* @brief Invalid magic for EPM */

#define kEPMMagicError "EPM??"

///! @brief Version kind enum.
///! @brief Use in boot block version field.

enum kEPMKind
{
	kEPMMpUx  = 0xcf, // Bridge or other embedded OS
	kEPMLinux = 0x8f,
	kEPMBSD	  = 0x9f,
	kEPMNewOS = 0x1f, // This kernel.
};

typedef struct BootBlock BootBlockType;

#ifdef __NEWOS_AMD64__
#define kEPMMagic kEPMMagic86
#else
#ifdef __NEWOS_ARM64__
#define kEPMMagic kEPMMagicARM
#else
#define kEPMMagic kEPMMagicError
#endif
#endif

///! @brief partition must start at this address.
///! Anything below is reserved for Data backup by the Main OS.
#define kEPMStartPartitionBlk (sizeof(BootBlock))

///! @brief Current EPM revision
#define kEPMRevision (0xAD)
///! @brief Current EPM revision
#define kEPMRevisionUEFI (0xAF)
/// !@brief EPM base address
#define kEpmBase (0U)

/// END OF SPECS

#endif // ifndef __FIRMWARE_EPM_HXX__
