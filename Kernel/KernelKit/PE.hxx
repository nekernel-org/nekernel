/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: PE.hxx
	Purpose: Portable Executable for Kernel.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __PE__
#define __PE__

#include <NewKit/Defines.hpp>

#define kPeMagic 0x00004550

typedef struct ExecHeader final
{
	Kernel::UInt32 mMagic; // PE\0\0 or 0x00004550
	Kernel::UInt16 mMachine;
	Kernel::UInt16 mNumberOfSections;
	Kernel::UInt32 mTimeDateStamp;
	Kernel::UInt32 mPointerToSymbolTable;
	Kernel::UInt32 mNumberOfSymbols;
	Kernel::UInt16 mSizeOfOptionalHeader;
	Kernel::UInt16 mCharacteristics;
} PACKED ExecHeader, *ExecHeaderPtr;

#define kMagPE32 0x010b
#define kMagPE64 0x020b

#define kPEMachineAMD64 0x8664
#define kPEMachineARM64 0xaa64

typedef struct ExecOptionalHeader final
{
	Kernel::UInt16	mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	Kernel::UChar	mMajorLinkerVersion;
	Kernel::UChar	mMinorLinkerVersion;
	Kernel::UIntPtr mSizeOfCode;
	Kernel::UIntPtr mSizeOfInitializedData;
	Kernel::UIntPtr mSizeOfUninitializedData;
	Kernel::UInt32	mAddressOfEntryPoint;
	Kernel::UInt32	mBaseOfCode;
	Kernel::UIntPtr mImageBase;
	Kernel::UInt32	mSectionAlignment;
	Kernel::UInt32	mFileAlignment;
	Kernel::UInt16	mMajorOperatingSystemVersion;
	Kernel::UInt16	mMinorOperatingSystemVersion;
	Kernel::UInt16	mMajorImageVersion;
	Kernel::UInt16	mMinorImageVersion;
	Kernel::UInt16	mMajorSubsystemVersion;
	Kernel::UInt16	mMinorSubsystemVersion;
	Kernel::UInt32	mWin32VersionValue;
	Kernel::UIntPtr mSizeOfImage;
	Kernel::UIntPtr mSizeOfHeaders;
	Kernel::UInt32	mCheckSum;
	Kernel::UInt16	mSubsystem;
	Kernel::UInt16	mDllCharacteristics;
	Kernel::UIntPtr mSizeOfStackReserve;
	Kernel::UIntPtr mSizeOfStackCommit;
	Kernel::UIntPtr mSizeOfHeapReserve;
	Kernel::UIntPtr mSizeOfHeapCommit;
	Kernel::UInt32	mLoaderFlags;
	Kernel::UInt32	mNumberOfRvaAndSizes;
} PACKED ExecOptionalHeader, *ExecOptionalHeaderPtr;

typedef struct ExecSectionHeader final
{
	CONST Kernel::UChar mName[8];
	Kernel::UInt32		mVirtualSize;
	Kernel::UInt32		mVirtualAddress;
	Kernel::UInt32		mSizeOfRawData;
	Kernel::UInt32		mPointerToRawData;
	Kernel::UInt32		mPointerToRelocations;
	Kernel::UInt32		mPointerToLinenumbers;
	Kernel::UInt16		mNumberOfRelocations;
	Kernel::UInt16		mNumberOfLinenumbers;
	Kernel::UInt32		mCharacteristics;
} PACKED ExecSectionHeader, *ExecSectionHeaderPtr;

enum kExecDataDirParams
{
	kExecExport,
	kExecImport,
	kExecInvalid,
	kExecCount,
};

typedef struct ExecExportDirectory
{
	Kernel::UInt32 mCharacteristics;
	Kernel::UInt32 mTimeDateStamp;
	Kernel::UInt16 mMajorVersion;
	Kernel::UInt16 mMinorVersion;
	Kernel::UInt32 mName;
	Kernel::UInt32 mBase;
	Kernel::UInt32 mNumberOfFunctions;
	Kernel::UInt32 mNumberOfNames;
	Kernel::UInt32 mAddressOfFunctions; // export table rva
	Kernel::UInt32 mAddressOfNames;
	Kernel::UInt32 mAddressOfNameOrdinal; // ordinal table rva
} PACKED ExecExportDirectory, *ExecExportDirectoryPtr;

typedef struct ExecImportDirectory
{
	union {
		Kernel::UInt32 mCharacteristics;
		Kernel::UInt32 mOriginalFirstThunk;
	};
	Kernel::UInt32 mTimeDateStamp;
	Kernel::UInt32 mForwarderChain;
	Kernel::UInt32 mNameRva;
	Kernel::UInt32 mThunkTableRva;
} PACKED ExecImportDirectory, *ExecImportDirectoryPtr;

#define kPeStart "__ImageStart"

#endif /* ifndef __PE__ */
