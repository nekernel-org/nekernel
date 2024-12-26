/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

	Revision History:

	??/??/24: Added file (amlel)
	23 Jul 24: Update filename to Defines.h and using ALIGN_NVME for NVME structs. (amlel)

------------------------------------------- */

#ifndef __MODULE_NVME_H__
#define __MODULE_NVME_H__

#include <NewKit/Defines.h>

/// TODO: checklist in: https://wiki.osdev.org/NVMe

#define ALIGN_NVME ATTRIBUTE(aligned(sizeof(Kernel::UInt32)))

namespace Kernel
{
	struct ALIGN_NVME HAL_NVME_BAR_0 final
	{
		UInt32 fCapabilities;
		UInt32 fVersion;
		UInt32 fIntMaskSet;
		UInt32 fIntMaskClr;
		UInt32 fContrlConf;
		UInt32 fContrlStat;
		UInt32 fAdminQueueAttr;
		UInt32 fAdminSubmissionQueue;
		UInt32 fAdminCompletionQueue;
	};

	struct ALIGN_NVME HAL_NVME_QUEUE final
	{
		UInt32 fOpcode;
		UInt32 fNSID;
		UInt32 fReserved[3];
		UInt32 fMetadataPtr[5];
		UInt32 fDataPtr[9];
		UInt32 CommandSpecific[15];
	};

	enum
	{
		kCreateCompletionQueueNVME = 0x05,
		kCreateSubmissionQueueNVME = 0x01,
		kIdentifyNVME			   = 0x06,
		kReadNVME				   = 0x02,
		kWriteNVME				   = 0x01,
	};

	/// @brief Creates an admin command for a DMA operation.
	template <Int32 Opcode>
	inline Bool nvme_create_admin_command(HAL_NVME_QUEUE* entry, UInt32 nsid, UInt32 prpTransfer[3], UInt32 startingLba[2], UInt32 lowTransferBlocks)
	{
		if (entry == nullptr)
			return false;

		entry->CommandSpecific[9]  = startingLba[0];
		entry->CommandSpecific[10] = startingLba[1];

		entry->CommandSpecific[11] = lowTransferBlocks;

		entry->CommandSpecific[5] = prpTransfer[0];
		entry->CommandSpecific[6] = prpTransfer[1];
		entry->CommandSpecific[7] = prpTransfer[2];

		entry->CommandSpecific[0] = nsid;

		return true;
	}

	/// @brief Creates an I/O command for a DMA operation.
	template <Int32 Opcode>
	inline Bool nvme_create_io_command(HAL_NVME_QUEUE* entry, UInt64 baseAddress, UInt32 identLoAndQueueSizeHi, UInt32 flagsLoAndQueueComplIdHi, UInt32 identify, Bool provideIdentify = false, Bool namespaceIdentify = false)
	{
		if (entry == nullptr)
			return false;

		if (baseAddress == 0)
			return false;

		entry->fOpcode = Opcode;

		entry->CommandSpecific[5] = (baseAddress & 0xFF);
		entry->CommandSpecific[6] = static_cast<UInt32>(baseAddress);

		if (!provideIdentify)
		{
			entry->CommandSpecific[9]  = identLoAndQueueSizeHi;
			entry->CommandSpecific[10] = flagsLoAndQueueComplIdHi;
		}
		else
		{
			entry->CommandSpecific[9] = identify;

			if (namespaceIdentify)
			{
				entry->CommandSpecific[0] = 1;
			}
		}

		// Use (1 << 0) as contigunous is better supported.

		return true;
	}
} // namespace Kernel

#endif // ifndef __MODULE_NVME_H__
