/* -------------------------------------------

    Copyright Mahrouss Logic

    File: PE.hpp
    Purpose: Portable Executable for HCore.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __PE__
#define __PE__

#include <NewKit/Defines.hpp>
#include <KernelKit/PE.hpp>

typedef HCore::UIntPtr U64;
typedef HCore::UInt32 U32;
typedef HCore::UInt16 U16;
typedef HCore::UInt8 U8;
typedef U8 BYTE;

#define kPeMagic 0x00004550

typedef struct ExecHeader final {
  U32 mMagic;  // PE\0\0 or 0x00004550
  U16 mMachine;
  U16 mNumberOfSections;
  U32 mTimeDateStamp;
  U32 mPointerToSymbolTable;
  U32 mNumberOfSymbols;
  U16 mSizeOfOptionalHeader;
  U16 mCharacteristics;
} ALIGN(8) ExecHeader, *ExecHeaderPtr;

#define kMagPE32 0x010b
#define kMagPE64 0x020b

#define kPEMachineAMD64 0x8664
#define kPEMachineARM64 0xaa64

typedef struct ExecOptionalHeader final {
  U16 mMagic;  // 0x010b - PE32, 0x020b - PE32+ (64 bit)
  U8 mMajorLinkerVersion;
  U8 mMinorLinkerVersion;
  U64 mSizeOfCode;
  U64 mSizeOfInitializedData;
  U64 mSizeOfUninitializedData;
  U32 mAddressOfEntryPoint;
  U32 mBaseOfCode;
  U64 mImageBase;
  U32 mSectionAlignment;
  U32 mFileAlignment;
  U16 mMajorOperatingSystemVersion;
  U16 mMinorOperatingSystemVersion;
  U16 mMajorImageVersion;
  U16 mMinorImageVersion;
  U16 mMajorSubsystemVersion;
  U16 mMinorSubsystemVersion;
  U32 mWin32VersionValue;
  U64 mSizeOfImage;
  U64 mSizeOfHeaders;
  U32 mCheckSum;
  U16 mSubsystem;
  U16 mDllCharacteristics;
  U64 mSizeOfStackReserve;
  U64 mSizeOfStackCommit;
  U64 mSizeOfHeapReserve;
  U64 mSizeOfHeapCommit;
  U32 mLoaderFlags;
  U32 mNumberOfRvaAndSizes;
} ExecOptionalHeader, *ExecOptionalHeaderPtr;

typedef struct ExecSectionHeader final {
  BYTE mName[8];
  U32 mVirtualSize;
  U32 mVirtualAddress;
  U32 mSizeOfRawData;
  U32 mPointerToRawData;
  U32 mPointerToRelocations;
  U32 mPointerToLinenumbers;
  U16 mNumberOfRelocations;
  U16 mNumberOfLinenumbers;
  U32 mCharacteristics;
} ExecSectionHeader, *ExecSectionHeaderPtr;

enum kExecDataDirParams {
  kExecExport,
  kExecImport,
  kExecCnt,
};

typedef struct ExecExportDirectory {
  U32 mCharacteristics;
  U32 mTimeDateStamp;
  U16 mMajorVersion;
  U16 mMinorVersion;
  U32 mName;
  U32 mBase;
  U32 mNumberOfFunctions;
  U32 mNumberOfNames;
  U32 mAddressOfFunctions;  // export table rva
  U32 mAddressOfNames;
  U32 mAddressOfNameOrdinal;  // ordinal table rva
} ExecExportDirectory, *ExecExportDirectoryPtr;

typedef struct ExecImportDirectory {
  union {
    U32 mCharacteristics;
    U32 mOriginalFirstThunk;
  };
  U32 mTimeDateStamp;
  U32 mForwarderChain;
  U32 mNameRva;
  U32 mThunkTableRva;
} ExecImportDirectory, *ExecImportDirectoryPtr;

#define kPeStart "__hcore_subsys_start"

#endif /* ifndef __PE__ */
