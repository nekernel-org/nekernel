/* -------------------------------------------

	Copyright ZKA Technologies

	File: PEF.hxx
	Purpose: Preferred Executable Format for Kernel.

	Revision History:

	?/?/23: Added file (amlel)

------------------------------------------- */

#ifndef __KERNELKIT_INC_PEF_HXX__
#define __KERNELKIT_INC_PEF_HXX__

#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/LoaderInterface.hxx>
#include <NewKit/Defines.hxx>

#define kPefMagic	 "Joy!"
#define kPefMagicFat "yoJ!"

#define kPefMagicLen 5

#define kPefVersion 3
#define kPefNameLen 255

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
		kPefSubArchGeneric,
		kPefSubArchIBM,
	};

	enum
	{
		kPefKindExec		 = 1, /* .exe */
		kPefKindSharedObject = 2, /* .lib */
		kPefKindObject		 = 4, /* .obj */
		kPefKindDebug		 = 5, /* .dbg */
		kPefKindDriver		 = 6,
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

/* not mandatory, only for non fork based filesystems */
#define kPefExt		  ".exe"
#define kPefDylibExt  ".dll"
#define kPefLibExt	  ".lib"
#define kPefObjectExt ".obj"
#define kPefDebugExt  ".dbg"

// Kernel System Binary Interface.
#define kPefAbi 	   (0x5046)

#define kPefBaseOrigin (0x40000000)

#define kPefStart 		"__ImageStart"

#define kPefForkKind	kPefMagic
#define kPefForkKindFAT kPefMagicFat

#endif /* ifndef __KERNELKIT_INC_PEF_HXX__ */
