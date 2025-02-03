/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: PEF.h
	Purpose: Preferred Executable Format for Kernel.

	Revision History:

	?/?/23: Added file (amlel)

------------------------------------------- */

#ifndef KERNELKIT_PEF_H
#define KERNELKIT_PEF_H

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/LoaderInterface.h>
#include <NewKit/Defines.h>

#define kPefMagic	 "Joy!"
#define kPefMagicFat "yoJ!"

#define kPefMagicLen 5

#define kPefVersion 3
#define kPefNameLen 255

/* not mandatory, only for non fork based filesystems. */
#define kPefExt		  ".o"
#define kPefDylibExt  ".dylib"
#define kPefLibExt	  ".lib"
#define kPefObjectExt ".obj"
#define kPefDebugExt  ".dbg"
#define kPefDriverExt ".sys"

// Kernel System Binary Interface.
#define kPefAbi (0x5046)

#define kPefBaseOrigin (0x40000000)

#define kPefStart "__ImageStart"

#define kPefForkKind	kPefMagic
#define kPefForkKindFAT kPefMagicFat

namespace Kernel
{
	enum
	{
		kPefArchIntel86S,
		kPefArchAMD64,
		kPefArchRISCV,
		kPefArch64x0, /* 64x0. ISA */
		kPefArch32x0, /* 32x0. ISA */
		kPefArchPowerPC,
		kPefArchARM64,
		kPefArchCount	= (kPefArchPowerPC - kPefArchIntel86S) + 1,
		kPefArchInvalid = 0xFF,
	};

	enum
	{
		kPefSubArchAMD,
		kPefSubArchIntel,
		kPefSubArchARM,
		kPefSubArchGeneric,
		kPefSubArchIBM,
	};

	enum
	{
		kPefKindExec   = 1, /* .o */
		kPefKindDylib  = 2, /* .dylib */
		kPefKindObject = 4, /* .obj */
		kPefKindDebug  = 5, /* .dbg */
		kPefKindDriver = 6,
		kPefKindCount,
	};

	typedef struct PEFContainer final
	{
		Char	Magic[kPefMagicLen];
		UInt32	Linker;
		UInt32	Version;
		UInt32	Kind;
		UInt32	Abi;
		UInt32	Cpu;
		UInt32	SubCpu; /* Cpu specific information */
		UIntPtr Start;
		SizeT	HdrSz; /* Size of header */
		SizeT	Count; /* container header count */
	} PACKED PEFContainer;

	/* First PEFCommandHeader starts after PEFContainer */

	typedef struct PEFCommandHeader final
	{
		Char	Name[kPefNameLen]; /* container name */
		UInt32	Cpu;			   /* container cpu */
		UInt32	SubCpu;			   /* container sub-cpu */
		UInt32	Flags;			   /* container flags */
		UInt16	Kind;			   /* container kind */
		UIntPtr Offset;			   /* content offset */
		SizeT	Size;			   /* content Size */
	} PACKED PEFCommandHeader;

	enum
	{
		kPefCode	 = 0xC,
		kPefData	 = 0xD,
		kPefZero	 = 0xE,
		kPefLinkerID = 0x1,
	};
} // namespace Kernel

#endif /* ifndef KERNELKIT_PEF_H */
