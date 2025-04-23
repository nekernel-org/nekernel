/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <FirmwareKit/EFI/EFI.h>

#define kSectorAlignGPT_PartTbl	  (420U)
#define kSectorAlignGPT_PartEntry (72U)
#define kMagicLenGPT			  (8U)
#define kMagicGPT				  ("EFI PART") // "EFI PART"
#define kGPTPartitionTableLBA	  (512U + sizeof(GPT_PARTITION_TABLE))

namespace Kernel
{
	struct GPT_PARTITION_TABLE;
	struct GPT_PARTITION_ENTRY;

	struct PACKED GPT_PARTITION_TABLE final
	{
		Char	Signature[kMagicLenGPT];
		UInt32	Revision;
		UInt32	HeaderSize;
		UInt32	CRC32;
		UInt32	Reserved1;
		UInt64	LBAHeader;
		UInt64	LBAAltHeader;
		UInt64	FirstGPTEntry;
		UInt64	LastGPTEntry;
		EfiGUID Guid;
		UInt64	StartingLBA;
		UInt32	NumPartitionEntries;
		UInt32	SizeOfEntries;
		UInt32	CRC32PartEntry;
		UInt8	Reserved2[kSectorAlignGPT_PartTbl];
	};

	struct PACKED GPT_PARTITION_ENTRY
	{
		EfiGUID PartitionTypeGUID;
		EfiGUID UniquePartitionGUID;
		UInt64	StartLBA;
		UInt64	EndLBA;
		UInt64	Attributes;
		UInt8	Name[kSectorAlignGPT_PartEntry];
	};
} // namespace Kernel
