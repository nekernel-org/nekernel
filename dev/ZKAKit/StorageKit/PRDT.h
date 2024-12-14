/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/PCI/DMA.h>
#include <KernelKit/PCI/Iterator.h>
#include <NewKit/Ref.h>

#define kPrdtTransferSize (sizeof(Kernel::UShort))

namespace Kernel
{
	/// @brief Tranfer information about PRD.
	enum kPRDTTransfer
	{
		kPRDTTransferInProgress,
		kPRDTTransferIsDone,
		kPRDTTransferCount,
	};

	/// @brief Physical Region Descriptor Table.
	struct PRDT
	{
		UInt32 fPhysAddress;
		UInt32 fSectorCount;
		UInt8  fEndBit;
	};

	void construct_prdt(Ref<PRDT>& prd);

	EXTERN_C Int32 kPRDTTransferStatus;
} // namespace Kernel
