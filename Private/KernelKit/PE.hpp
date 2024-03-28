/* -------------------------------------------

    Copyright Mahrouss Logic

    File: PE.hpp
    Purpose: Portable Executable for NewOS.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __PE__
#define __PE__

#include <NewKit/Defines.hpp>
#include <KernelKit/PE.hpp>

namespace Detail
{
  typedef NewOS::UIntPtr PE_QWORD;
  typedef NewOS::UInt32 PE_DWORD;
  typedef NewOS::UInt16 PE_WORD;
  typedef NewOS::UChar PE_BYTE;
} // namespace Detail

#define kPeMagic 0x00004550

typedef struct ExecHeader final {
  Detail::PE_DWORD mMagic;  // PE\0\0 or 0x00004550
  Detail::PE_WORD mMachine;
  Detail::PE_WORD mNumberOfSections;
  Detail::PE_DWORD mTimeDateStamp;
  Detail::PE_DWORD mPointerToSymbolTable;
  Detail::PE_DWORD mNumberOfSymbols;
  Detail::PE_WORD mSizeOfOptionalHeader;
  Detail::PE_WORD mCharacteristics;
} ALIGN(8) ExecHeader, *ExecHeaderPtr;

#define kMagPE32 0x010b
#define kMagPE64 0x020b

#define kPEMachineAMD64 0x8664
#define kPEMachineARM64 0xaa64

typedef struct ExecOptionalHeader final {
  Detail::PE_WORD mMagic;  // 0x010b - PE32, 0x020b - PE32+ (64 bit)
  Detail::PE_BYTE mMajorLinkerVersion;
  Detail::PE_BYTE mMinorLinkerVersion;
  Detail::PE_QWORD mSizeOfCode;
  Detail::PE_QWORD mSizeOfInitializedData;
  Detail::PE_QWORD mSizeOfUninitializedData;
  Detail::PE_DWORD mAddressOfEntryPoint;
  Detail::PE_DWORD mBaseOfCode;
  Detail::PE_QWORD mImageBase;
  Detail::PE_DWORD mSectionAlignment;
  Detail::PE_DWORD mFileAlignment;
  Detail::PE_WORD mMajorOperatingSystemVersion;
  Detail::PE_WORD mMinorOperatingSystemVersion;
  Detail::PE_WORD mMajorImageVersion;
  Detail::PE_WORD mMinorImageVersion;
  Detail::PE_WORD mMajorSubsystemVersion;
  Detail::PE_WORD mMinorSubsystemVersion;
  Detail::PE_DWORD mWin32VersionValue;
  Detail::PE_QWORD mSizeOfImage;
  Detail::PE_QWORD mSizeOfHeaders;
  Detail::PE_DWORD mCheckSum;
  Detail::PE_WORD mSubsystem;
  Detail::PE_WORD mDllCharacteristics;
  Detail::PE_QWORD mSizeOfStackReserve;
  Detail::PE_QWORD mSizeOfStackCommit;
  Detail::PE_QWORD mSizeOfHeapReserve;
  Detail::PE_QWORD mSizeOfHeapCommit;
  Detail::PE_DWORD mLoaderFlags;
  Detail::PE_DWORD mNumberOfRvaAndSizes;
} ExecOptionalHeader, *ExecOptionalHeaderPtr;

typedef struct ExecSectionHeader final {
  CONST Detail::PE_BYTE mName[8];
  Detail::PE_DWORD mVirtualSize;
  Detail::PE_DWORD mVirtualAddress;
  Detail::PE_DWORD mSizeOfRawData;
  Detail::PE_DWORD mPointerToRawData;
  Detail::PE_DWORD mPointerToRelocations;
  Detail::PE_DWORD mPointerToLinenumbers;
  Detail::PE_WORD mNumberOfRelocations;
  Detail::PE_WORD mNumberOfLinenumbers;
  Detail::PE_DWORD mCharacteristics;
} ExecSectionHeader, *ExecSectionHeaderPtr;

enum kExecDataDirParams {
  kExecExport,
  kExecImport,
  kExecInvalid,
  kExecCount,
};

typedef struct ExecExportDirectory {
  Detail::PE_DWORD mCharacteristics;
  Detail::PE_DWORD mTimeDateStamp;
  Detail::PE_WORD mMajorVersion;
  Detail::PE_WORD mMinorVersion;
  Detail::PE_DWORD mName;
  Detail::PE_DWORD mBase;
  Detail::PE_DWORD mNumberOfFunctions;
  Detail::PE_DWORD mNumberOfNames;
  Detail::PE_DWORD mAddressOfFunctions;  // export table rva
  Detail::PE_DWORD mAddressOfNames;
  Detail::PE_DWORD mAddressOfNameOrdinal;  // ordinal table rva
} ExecExportDirectory, *ExecExportDirectoryPtr;

typedef struct ExecImportDirectory {
  union {
    Detail::PE_DWORD mCharacteristics;
    Detail::PE_DWORD mOriginalFirstThunk;
  };
  Detail::PE_DWORD mTimeDateStamp;
  Detail::PE_DWORD mForwarderChain;
  Detail::PE_DWORD mNameRva;
  Detail::PE_DWORD mThunkTableRva;
} ExecImportDirectory, *ExecImportDirectoryPtr;

#define kPeStart "__hcore_subsys_start"

#endif /* ifndef __PE__ */
