/* -------------------------------------------

	Copyright ZKA Technologies

	File: PE.hxx
	Purpose: Portable Executable for Kernel.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __KERNELKIT_INC_PE_HXX__
#define __KERNELKIT_INC_PE_HXX__

#include <NewKit/Defines.hxx>

#define kPeMagic 0x00004550

#define kMagPE32 0x010b
#define kMagPE64 0x020b

#define kPeMachineAMD64 0x8664
#define kPeMachineARM64 0xaa64

typedef struct ExecHeader final
{
	Kernel::UInt32 mSignature;
	Kernel::UInt16 mMachine;
	Kernel::UInt16 mNumberOfSections;
	Kernel::UInt32 mTimeDateStamp;
	Kernel::UInt32 mPointerToSymbolTable;
	Kernel::UInt32 mNumberOfSymbols;
	Kernel::UInt16 mSizeOfOptionalHeader;
	Kernel::UInt16 mCharacteristics;
}  ExecHeader, *ExecHeaderPtr;

typedef struct ExecOptionalHeader final
{
	Kernel::UInt16 mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	Kernel::UInt8  mMajorLinkerVersion;
	Kernel::UInt8  mMinorLinkerVersion;
	Kernel::UInt32 mSizeOfCode;
	Kernel::UInt32 mSizeOfInitializedData;
	Kernel::UInt32 mSizeOfUninitializedData;
	Kernel::UInt32 mAddressOfEntryPoint;
	Kernel::UInt32 mBaseOfCode;
	Kernel::UInt32 mBaseOfData;
	Kernel::UInt32 mImageBase;
	Kernel::UInt32 mSectionAlignment;
	Kernel::UInt32 mFileAlignment;
	Kernel::UInt16 mMajorOperatingSystemVersion;
	Kernel::UInt16 mMinorOperatingSystemVersion;
	Kernel::UInt16 mMajorImageVersion;
	Kernel::UInt16 mMinorImageVersion;
	Kernel::UInt16 mMajorSubsystemVersion;
	Kernel::UInt16 mMinorSubsystemVersion;
	Kernel::UInt32 mWin32VersionValue;
	Kernel::UInt32 mSizeOfImage;
	Kernel::UInt32 mSizeOfHeaders;
	Kernel::UInt32 mCheckSum;
	Kernel::UInt16 mSubsystem;
	Kernel::UInt16 mDllCharacteristics;
	Kernel::UInt32 mSizeOfStackReserve;
	Kernel::UInt32 mSizeOfStackCommit;
	Kernel::UInt32 mSizeOfHeapReserve;
	Kernel::UInt32 mSizeOfHeapCommit;
	Kernel::UInt32 mLoaderFlags;
	Kernel::UInt32 mNumberOfRvaAndSizes;
}  ExecOptionalHeader, *ExecOptionalHeaderPtr;

typedef struct ExecSectionHeader final
{
	Kernel::Char       mName[8];
	Kernel::UInt32		mVirtualSize;
	Kernel::UInt32		mVirtualAddress;
	Kernel::UInt32		mSizeOfRawData;
	Kernel::UInt32		mPointerToRawData;
	Kernel::UInt32		mPointerToRelocations;
	Kernel::UInt32		mPointerToLineNumbers;
	Kernel::UInt16		mNumberOfRelocations;
	Kernel::UInt16		mNumberOfLinenumbers;
	Kernel::UInt32		mCharacteristics;
}  ExecSectionHeader, *ExecSectionHeaderPtr;

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
}  ExecExportDirectory, *ExecExportDirectoryPtr;

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
}  ExecImportDirectory, *ExecImportDirectoryPtr;

typedef struct ExecDataDirectory {
  Kernel::UInt32 VirtualAddress;
  Kernel::UInt32 Size;
} ExecDataDirectory, *ExecDataDirectoryPtr;

typedef struct ExecImageHeader {
	ExecHeader mHeader;
	ExecOptionalHeader mOptHdr;
} ExecImageHeader, *ExecImageHeaderPtr;

enum
{
	eUserSection = 0x00000020,
};

#endif /* ifndef __KERNELKIT_INC_PE_HXX__ */
