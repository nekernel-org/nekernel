/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <KernelKit/PCI/DMA.hxx>
#include <KernelKit/PCI/Iterator.hxx>
#include <NewKit/Ref.hxx>

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
