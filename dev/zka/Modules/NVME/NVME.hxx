/* -------------------------------------------

	Copyright ZKA Technologies.

	Revision History:

	??/??/24: Added file (amlel)
	23 Jul 24: Update filename to Defines.hxx and using ALIGN_NVME for NVME structs. (amlel)

------------------------------------------- */

#ifndef __MODULE_NVME_HXX__
#define __MODULE_NVME_HXX__

#include <NewKit/Defines.hxx>

/// TODO: checklist in: https://wiki.osdev.org/NVMe

#define ALIGN_NVME ATTRIBUTE(aligned(sizeof(Kernel::NVMEInt32)))

namespace Kernel
{
	typedef UInt32 NVMEInt32;

	struct ALIGN_NVME NVMEBar0 final
	{
		NVMEInt32 fCapabilities;
		NVMEInt32 fVersion;
		NVMEInt32 fIntMaskSet;
		NVMEInt32 fIntMaskClr;
		NVMEInt32 fContrlConf;
		NVMEInt32 fContrlStat;
		NVMEInt32 fAdminQueueAttr;
		NVMEInt32 fAdminSubmissionQueue;
		NVMEInt32 fAdminCompletionQueue;
	};

	struct ALIGN_NVME NVMEQueue final
	{
		NVMEInt32 fOpcode;
		NVMEInt32 fNSID;
		NVMEInt32 fReserved[3];
		NVMEInt32 fMetadataPtr[5];
		NVMEInt32 fDataPtr[9];
		NVMEInt32 CommandSpecific[15];
	};

	enum
	{
		eCreateCompletionQueueNVME = 0x05,
		eCreateSubmissionQueueNVME = 0x01,
		eIdentifyNVME			   = 0x06,
		eReadNVME				   = 0x02,
		eWriteNVME				   = 0x01,
	};

	template <Int32 Opcode>
	inline Bool nvme_create_admin_command(NVMEQueue* entry, UInt32 nsid, UInt32 prpTransfer[3], UInt32 startingLba[2], UInt32 lowTransferBlocks)
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

	template <Int32 Opcode>
	inline Bool nvme_create_io_command(NVMEQueue* entry, UInt64 baseAddress, UInt32 identLoAndQueueSizeHi, UInt32 flagsLoAndQueueComplIdHi, UInt32 identify, Bool provideIdentify = false, Bool namespaceIdentify = false)
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

		// use (1 << 0) as contigunous is better supported.

		return true;
	}
} // namespace Kernel

#endif // ifndef __MODULE_NVME_HXX__
