/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: PE.h
	Purpose: Portable Executable for Kernel.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __KERNELKIT_INC_PE_H__
#define __KERNELKIT_INC_PE_H__

#include <NewKit/Defines.h>

#define kPeSignature 0x00004550

#define kPeDLLBase 0x4000000
#define kPeEXEBase 0x1000000

#define kPeMagic32 0x010b
#define kPeMagic64 0x020b

#define kPeMachineAMD64 0x8664
#define kPeMachineARM64 0xaa64

typedef struct LDR_EXEC_HEADER final
{
	NeOS::UInt32 mSignature;
	NeOS::UInt16 mMachine;
	NeOS::UInt16 mNumberOfSections;
	NeOS::UInt32 mTimeDateStamp;
	NeOS::UInt32 mPointerToSymbolTable;
	NeOS::UInt32 mNumberOfSymbols;
	NeOS::UInt16 mSizeOfOptionalHeader;
	NeOS::UInt16 mCharacteristics;
} LDR_EXEC_HEADER, *LDR_EXEC_HEADER_PTR;

typedef struct LDR_OPTIONAL_HEADER final
{
	NeOS::UInt16 mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	NeOS::UInt8	 mMajorLinkerVersion;
	NeOS::UInt8	 mMinorLinkerVersion;
	NeOS::UInt32 mSizeOfCode;
	NeOS::UInt32 mSizeOfInitializedData;
	NeOS::UInt32 mSizeOfUninitializedData;
	NeOS::UInt32 mAddressOfEntryPoint;
	NeOS::UInt32 mBaseOfCode;
	NeOS::UInt32 mBaseOfData;
	NeOS::UInt32 mImageBase;
	NeOS::UInt32 mSectionAlignment;
	NeOS::UInt32 mFileAlignment;
	NeOS::UInt16 mMajorOperatingSystemVersion;
	NeOS::UInt16 mMinorOperatingSystemVersion;
	NeOS::UInt16 mMajorImageVersion;
	NeOS::UInt16 mMinorImageVersion;
	NeOS::UInt16 mMajorSubsystemVersion;
	NeOS::UInt16 mMinorSubsystemVersion;
	NeOS::UInt32 mWin32VersionValue;
	NeOS::UInt32 mSizeOfImage;
	NeOS::UInt32 mSizeOfHeaders;
	NeOS::UInt32 mCheckSum;
	NeOS::UInt16 mSubsystem;
	NeOS::UInt16 mDllCharacteristics;
	NeOS::UInt32 mSizeOfStackReserve;
	NeOS::UInt32 mSizeOfStackCommit;
	NeOS::UInt32 mSizeOfHeapReserve;
	NeOS::UInt32 mSizeOfHeapCommit;
	NeOS::UInt32 mLoaderFlags;
	NeOS::UInt32 mNumberOfRvaAndSizes;
} LDR_OPTIONAL_HEADER, *LDR_OPTIONAL_HEADER_PTR;

typedef struct LDR_SECTION_HEADER final
{
	NeOS::Char	 mName[8];
	NeOS::UInt32 mVirtualSize;
	NeOS::UInt32 mVirtualAddress;
	NeOS::UInt32 mSizeOfRawData;
	NeOS::UInt32 mPointerToRawData;
	NeOS::UInt32 mPointerToRelocations;
	NeOS::UInt32 mPointerToLineNumbers;
	NeOS::UInt16 mNumberOfRelocations;
	NeOS::UInt16 mNumberOfLinenumbers;
	NeOS::UInt32 mCharacteristics;
} LDR_SECTION_HEADER, *LDR_SECTION_HEADER_PTR;

enum kExecDataDirParams
{
	kExecExport,
	kExecImport,
	kExecInvalid,
	kExecCount,
};

typedef struct LDR_EXPORT_DIRECTORY
{
	NeOS::UInt32 mCharacteristics;
	NeOS::UInt32 mTimeDateStamp;
	NeOS::UInt16 mMajorVersion;
	NeOS::UInt16 mMinorVersion;
	NeOS::UInt32 mName;
	NeOS::UInt32 mBase;
	NeOS::UInt32 mNumberOfFunctions;
	NeOS::UInt32 mNumberOfNames;
	NeOS::UInt32 mAddressOfFunctions; // export table rva
	NeOS::UInt32 mAddressOfNames;
	NeOS::UInt32 mAddressOfNameOrdinal; // ordinal table rva
} LDR_EXPORT_DIRECTORY, *LDR_EXPORT_DIRECTORY_PTR;

typedef struct LDR_IMPORT_DIRECTORY
{
	union {
		NeOS::UInt32 mCharacteristics;
		NeOS::UInt32 mOriginalFirstThunk;
	};
	NeOS::UInt32 mTimeDateStamp;
	NeOS::UInt32 mForwarderChain;
	NeOS::UInt32 mNameRva;
	NeOS::UInt32 mThunkTableRva;
} LDR_IMPORT_DIRECTORY, *LDR_IMPORT_DIRECTORY_PTR;

typedef struct LDR_DATA_DIRECTORY
{
	NeOS::UInt32 VirtualAddress;
	NeOS::UInt32 Size;
} LDR_DATA_DIRECTORY, *LDR_DATA_DIRECTORY_PTR;

typedef struct LDR_IMAGE_HEADER
{
	LDR_EXEC_HEADER		mHeader;
	LDR_OPTIONAL_HEADER mOptHdr;
} LDR_IMAGE_HEADER, *LDR_IMAGE_HEADER_PTR;

enum
{
	kUserSection  = 0x00000020,
	kPEResourceId = 0xFFaadd00,
};

#endif /* ifndef __KERNELKIT_INC_PE_H__ */
