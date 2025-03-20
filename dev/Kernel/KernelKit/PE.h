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
	NeOS::UInt32 Signature;
	NeOS::UInt16 Machine;
	NeOS::UInt16 NumberOfSections;
	NeOS::UInt32 TimeDateStamp;
	NeOS::UInt32 PointerToSymbolTable;
	NeOS::UInt32 NumberOfSymbols;
	NeOS::UInt16 SizeOfOptionalHeader;
	NeOS::UInt16 Characteristics;
} LDR_EXEC_HEADER, *LDR_EXEC_HEADER_PTR;

typedef struct LDR_OPTIONAL_HEADER final
{
	NeOS::UInt16 Magic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	NeOS::UInt8	 MajorLinkerVersion;
	NeOS::UInt8	 MinorLinkerVersion;
	NeOS::UInt32 SizeOfCode;
	NeOS::UInt32 SizeOfInitializedData;
	NeOS::UInt32 SizeOfUninitializedData;
	NeOS::UInt32 AddressOfEntryPoint;
	NeOS::UInt32 BaseOfCode;
	NeOS::UInt32 BaseOfData;
	NeOS::UInt32 ImageBase;
	NeOS::UInt32 SectionAlignment;
	NeOS::UInt32 FileAlignment;
	NeOS::UInt16 MajorOperatingSystemVersion;
	NeOS::UInt16 MinorOperatingSystemVersion;
	NeOS::UInt16 MajorImageVersion;
	NeOS::UInt16 MinorImageVersion;
	NeOS::UInt16 MajorSubsystemVersion;
	NeOS::UInt16 MinorSubsystemVersion;
	NeOS::UInt32 Win32VersionValue;
	NeOS::UInt32 SizeOfImage;
	NeOS::UInt32 SizeOfHeaders;
	NeOS::UInt32 CheckSum;
	NeOS::UInt16 Subsystem;
	NeOS::UInt16 DllCharacteristics;
	NeOS::UInt32 SizeOfStackReserve;
	NeOS::UInt32 SizeOfStackCommit;
	NeOS::UInt32 SizeOfHeapReserve;
	NeOS::UInt32 SizeOfHeapCommit;
	NeOS::UInt32 LoaderFlags;
	NeOS::UInt32 NumberOfRvaAndSizes;
} LDR_OPTIONAL_HEADER, *LDR_OPTIONAL_HEADER_PTR;

typedef struct LDR_SECTION_HEADER final
{
	NeOS::Char	 Name[8];
	NeOS::UInt32 VirtualSize;
	NeOS::UInt32 VirtualAddress;
	NeOS::UInt32 SizeOfRawData;
	NeOS::UInt32 PointerToRawData;
	NeOS::UInt32 PointerToRelocations;
	NeOS::UInt32 PointerToLineNumbers;
	NeOS::UInt16 NumberOfRelocations;
	NeOS::UInt16 NumberOfLinenumbers;
	NeOS::UInt32 Characteristics;
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
	NeOS::UInt32 Characteristics;
	NeOS::UInt32 TimeDateStamp;
	NeOS::UInt16 MajorVersion;
	NeOS::UInt16 MinorVersion;
	NeOS::UInt32 Name;
	NeOS::UInt32 Base;
	NeOS::UInt32 NumberOfFunctions;
	NeOS::UInt32 NumberOfNames;
	NeOS::UInt32 AddressOfFunctions; // export table rva
	NeOS::UInt32 AddressOfNames;
	NeOS::UInt32 AddressOfNameOrdinal; // ordinal table rva
} LDR_EXPORT_DIRECTORY, *LDR_EXPORT_DIRECTORY_PTR;

typedef struct LDR_IMPORT_DIRECTORY
{
	union {
		NeOS::UInt32 Characteristics;
		NeOS::UInt32 OriginalFirstThunk;
	};
	NeOS::UInt32 TimeDateStamp;
	NeOS::UInt32 ForwarderChain;
	NeOS::UInt32 NameRva;
	NeOS::UInt32 ThunkTableRva;
} LDR_IMPORT_DIRECTORY, *LDR_IMPORT_DIRECTORY_PTR;

typedef struct LDR_DATA_DIRECTORY
{
	NeOS::UInt32 VirtualAddress;
	NeOS::UInt32 Size;
} LDR_DATA_DIRECTORY, *LDR_DATA_DIRECTORY_PTR;

typedef struct LDR_IMAGE_HEADER
{
	LDR_EXEC_HEADER		Header;
	LDR_OPTIONAL_HEADER OptHdr;
} LDR_IMAGE_HEADER, *LDR_IMAGE_HEADER_PTR;

enum
{
	kUserSection  = 0x00000020,
	kPEResourceId = 0xFFaadd00,
};

#endif /* ifndef __KERNELKIT_INC_PE_H__ */
