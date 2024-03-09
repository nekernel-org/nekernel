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

namespace Detail
{
  typedef HCore::UIntPtr U64;
  typedef HCore::UInt32 U32;
  typedef HCore::UInt16 U16;
  typedef HCore::UInt8 U8;
  typedef Detail::U8 BYTE;
} // namespace Detail

#define kPeMagic 0x00004550

typedef struct ExecHeader final {
  Detail::U32 mMagic;  // PE\0\0 or 0x00004550
  Detail::U16 mMachine;
  Detail::U16 mNumberOfSections;
  Detail::U32 mTimeDateStamp;
  Detail::U32 mPointerToSymbolTable;
  Detail::U32 mNumberOfSymbols;
  Detail::U16 mSizeOfOptionalHeader;
  Detail::U16 mCharacteristics;
} ALIGN(8) ExecHeader, *ExecHeaderPtr;

#define kMagPE32 0x010b
#define kMagPE64 0x020b

#define kPEMachineAMD64 0x8664
#define kPEMachineARM64 0xaa64

typedef struct ExecOptionalHeader final {
  Detail::U16 mMagic;  // 0x010b - PE32, 0x020b - PE32+ (64 bit)
  Detail::U8 mMajorLinkerVersion;
  Detail::U8 mMinorLinkerVersion;
  Detail::U64 mSizeOfCode;
  Detail::U64 mSizeOfInitializedData;
  Detail::U64 mSizeOfUninitializedData;
  Detail::U32 mAddressOfEntryPoint;
  Detail::U32 mBaseOfCode;
  Detail::U64 mImageBase;
  Detail::U32 mSectionAlignment;
  Detail::U32 mFileAlignment;
  Detail::U16 mMajorOperatingSystemVersion;
  Detail::U16 mMinorOperatingSystemVersion;
  Detail::U16 mMajorImageVersion;
  Detail::U16 mMinorImageVersion;
  Detail::U16 mMajorSubsystemVersion;
  Detail::U16 mMinorSubsystemVersion;
  Detail::U32 mWin32VersionValue;
  Detail::U64 mSizeOfImage;
  Detail::U64 mSizeOfHeaders;
  Detail::U32 mCheckSum;
  Detail::U16 mSubsystem;
  Detail::U16 mDllCharacteristics;
  Detail::U64 mSizeOfStackReserve;
  Detail::U64 mSizeOfStackCommit;
  Detail::U64 mSizeOfHeapReserve;
  Detail::U64 mSizeOfHeapCommit;
  Detail::U32 mLoaderFlags;
  Detail::U32 mNumberOfRvaAndSizes;
} ExecOptionalHeader, *ExecOptionalHeaderPtr;

typedef struct ExecSectionHeader final {
  Detail::BYTE mName[8];
  Detail::U32 mVirtualSize;
  Detail::U32 mVirtualAddress;
  Detail::U32 mSizeOfRawData;
  Detail::U32 mPointerToRawData;
  Detail::U32 mPointerToRelocations;
  Detail::U32 mPointerToLinenumbers;
  Detail::U16 mNumberOfRelocations;
  Detail::U16 mNumberOfLinenumbers;
  Detail::U32 mCharacteristics;
} ExecSectionHeader, *ExecSectionHeaderPtr;

enum kExecDataDirParams {
  kExecExport,
  kExecImport,
  kExecInvalid,
  kExecCount,
};

typedef struct ExecExportDirectory {
  Detail::U32 mCharacteristics;
  Detail::U32 mTimeDateStamp;
  Detail::U16 mMajorVersion;
  Detail::U16 mMinorVersion;
  Detail::U32 mName;
  Detail::U32 mBase;
  Detail::U32 mNumberOfFunctions;
  Detail::U32 mNumberOfNames;
  Detail::U32 mAddressOfFunctions;  // export table rva
  Detail::U32 mAddressOfNames;
  Detail::U32 mAddressOfNameOrdinal;  // ordinal table rva
} ExecExportDirectory, *ExecExportDirectoryPtr;

typedef struct ExecImportDirectory {
  union {
    Detail::U32 mCharacteristics;
    Detail::U32 mOriginalFirstThunk;
  };
  Detail::U32 mTimeDateStamp;
  Detail::U32 mForwarderChain;
  Detail::U32 mNameRva;
  Detail::U32 mThunkTableRva;
} ExecImportDirectory, *ExecImportDirectoryPtr;

#define kPeStart "__hcore_subsys_start"

#endif /* ifndef __PE__ */
