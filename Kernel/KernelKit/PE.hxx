/* -------------------------------------------

    Copyright SoftwareLabs

    File: PE.hxx
    Purpose: Portable Executable for NewOS.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __PE__
#define __PE__

#include <NewKit/Defines.hpp>

#define kPeMagic 0x00004550

typedef struct ExecHeader final
{
	NewOS::UInt32 mMagic; // PE\0\0 or 0x00004550
	NewOS::UInt16 mMachine;
	NewOS::UInt16 mNumberOfSections;
	NewOS::UInt32 mTimeDateStamp;
	NewOS::UInt32 mPointerToSymbolTable;
	NewOS::UInt32 mNumberOfSymbols;
	NewOS::UInt16 mSizeOfOptionalHeader;
	NewOS::UInt16 mCharacteristics;
} ALIGN(8) ExecHeader, *ExecHeaderPtr;

#define kMagPE32 0x010b
#define kMagPE64 0x020b

#define kPEMachineAMD64 0x8664
#define kPEMachineARM64 0xaa64

typedef struct ExecOptionalHeader final
{
	NewOS::UInt16  mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	NewOS::UChar   mMajorLinkerVersion;
	NewOS::UChar   mMinorLinkerVersion;
	NewOS::UIntPtr mSizeOfCode;
	NewOS::UIntPtr mSizeOfInitializedData;
	NewOS::UIntPtr mSizeOfUninitializedData;
	NewOS::UInt32  mAddressOfEntryPoint;
	NewOS::UInt32  mBaseOfCode;
	NewOS::UIntPtr mImageBase;
	NewOS::UInt32  mSectionAlignment;
	NewOS::UInt32  mFileAlignment;
	NewOS::UInt16  mMajorOperatingSystemVersion;
	NewOS::UInt16  mMinorOperatingSystemVersion;
	NewOS::UInt16  mMajorImageVersion;
	NewOS::UInt16  mMinorImageVersion;
	NewOS::UInt16  mMajorSubsystemVersion;
	NewOS::UInt16  mMinorSubsystemVersion;
	NewOS::UInt32  mWin32VersionValue;
	NewOS::UIntPtr mSizeOfImage;
	NewOS::UIntPtr mSizeOfHeaders;
	NewOS::UInt32  mCheckSum;
	NewOS::UInt16  mSubsystem;
	NewOS::UInt16  mDllCharacteristics;
	NewOS::UIntPtr mSizeOfStackReserve;
	NewOS::UIntPtr mSizeOfStackCommit;
	NewOS::UIntPtr mSizeOfHeapReserve;
	NewOS::UIntPtr mSizeOfHeapCommit;
	NewOS::UInt32  mLoaderFlags;
	NewOS::UInt32  mNumberOfRvaAndSizes;
} ExecOptionalHeader, *ExecOptionalHeaderPtr;

typedef struct ExecSectionHeader final
{
	CONST NewOS::UChar mName[8];
	NewOS::UInt32	   mVirtualSize;
	NewOS::UInt32	   mVirtualAddress;
	NewOS::UInt32	   mSizeOfRawData;
	NewOS::UInt32	   mPointerToRawData;
	NewOS::UInt32	   mPointerToRelocations;
	NewOS::UInt32	   mPointerToLinenumbers;
	NewOS::UInt16	   mNumberOfRelocations;
	NewOS::UInt16	   mNumberOfLinenumbers;
	NewOS::UInt32	   mCharacteristics;
} ExecSectionHeader, *ExecSectionHeaderPtr;

enum kExecDataDirParams
{
	kExecExport,
	kExecImport,
	kExecInvalid,
	kExecCount,
};

typedef struct ExecExportDirectory
{
	NewOS::UInt32 mCharacteristics;
	NewOS::UInt32 mTimeDateStamp;
	NewOS::UInt16 mMajorVersion;
	NewOS::UInt16 mMinorVersion;
	NewOS::UInt32 mName;
	NewOS::UInt32 mBase;
	NewOS::UInt32 mNumberOfFunctions;
	NewOS::UInt32 mNumberOfNames;
	NewOS::UInt32 mAddressOfFunctions; // export table rva
	NewOS::UInt32 mAddressOfNames;
	NewOS::UInt32 mAddressOfNameOrdinal; // ordinal table rva
} ExecExportDirectory, *ExecExportDirectoryPtr;

typedef struct ExecImportDirectory
{
	union {
		NewOS::UInt32 mCharacteristics;
		NewOS::UInt32 mOriginalFirstThunk;
	};
	NewOS::UInt32 mTimeDateStamp;
	NewOS::UInt32 mForwarderChain;
	NewOS::UInt32 mNameRva;
	NewOS::UInt32 mThunkTableRva;
} ExecImportDirectory, *ExecImportDirectoryPtr;

#define kPeStart "__hcore_subsys_start"

#endif /* ifndef __PE__ */
