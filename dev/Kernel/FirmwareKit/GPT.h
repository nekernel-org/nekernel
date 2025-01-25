/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <FirmwareKit/EFI/EFI.h>

#define kSectorSizeGPT (512U)

namespace Kernel
{
	struct GPT_GUID;
	struct GPT_PARTITION_TABLE;
	struct GPT_PARTITION_ENTRY;

	/// @brief GPT GUID structure.
	typedef struct GPT_GUID
	{
		Kernel::UInt32 Data1;
		Kernel::UInt16 Data2;
		Kernel::UInt16 Data3;
		Kernel::UInt8  Data4[8];
	} GPT_GUID;

	struct PACKED GPT_PARTITION_TABLE final
	{
		Char	 PartitionName[8];
		UInt32	 Revision;
		UInt32	 HeaderSize;
		UInt32	 ChecksumCRC32;
		UInt32	 Reserved1;
		UInt64	 LBAHeader;
		UInt64	 LBAAltHeader;
		UInt64	 FirstGPTEntry;
		UInt64	 LastGPTEntry;
		GPT_GUID Guid;
		UInt64	 StartingLBA;
		UInt32	 NumPartitionEntries;
		UInt32	 SizeOfEntries;
		UInt32	 CRC32PartEntry;
		UInt8	 Reserved2[kSectorSizeGPT];
	};

	struct PACKED GPT_PARTITION_ENTRY
	{
		GPT_GUID PartitionTypeGUID;
		GPT_GUID UniquePartitionGUID;
		UInt64	 StartLBA;
		UInt64	 EndLBA;
		UInt64	 Attributes;
		UInt8	 Name[72];
	};
} // namespace Kernel
