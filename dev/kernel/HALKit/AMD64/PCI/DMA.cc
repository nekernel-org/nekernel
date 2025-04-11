/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/PCI/DMA.h>
#include <ArchKit/ArchKit.h>

namespace Kernel
{
	DMAWrapper::operator bool()
	{
		return this->fAddress;
	}

	bool DMAWrapper::operator!()
	{
		return !this->fAddress;
	}

	Boolean DMAWrapper::Check(UIntPtr offset) const
	{
		if (!this->fAddress)
			return false;

		if (offset == 0)
			return false;

		kout << "[DMAWrapper::IsIn] Checking offset...\r";
		return reinterpret_cast<UIntPtr>(this->fAddress) >= offset;
	}

	bool DMAWrapper::Write(UIntPtr& bit, const UInt32& offset)
	{
		kout << "[DMAWrapper::Read] Checking this->fAddress...\r";

		if (!this->fAddress)
			return false;

		(void)(kout << "[DMAWrapper::Write] Writing at address: " << hex_number(reinterpret_cast<UIntPtr>(this->fAddress) + offset) << kendl);

		ke_dma_write<UInt32>(reinterpret_cast<UIntPtr>(this->fAddress), offset, bit);

		return true;
	}

	UIntPtr DMAWrapper::Read(const UInt32& offset)
	{
		kout << "[DMAWrapper::Read] Checking this->fAddress...\r";

		if (!this->fAddress)
			return ~0;

		(void)(kout << "[DMAWrapper::Write] Writing at address: " << hex_number(reinterpret_cast<UIntPtr>(this->fAddress) + offset) << kendl);

		return (UIntPtr)ke_dma_read<UInt32>(reinterpret_cast<UIntPtr>(this->fAddress), offset);
	}

	UIntPtr DMAWrapper::operator[](UIntPtr& offset)
	{
		return this->Read(offset);
	}

	OwnPtr<IOBuf<Char*>> DMAFactory::Construct(OwnPtr<DMAWrapper>& dma)
	{
		if (!dma)
			return {};

		OwnPtr<IOBuf<Char*>> dmaOwnPtr =
			mm_make_own_ptr<IOBuf<Char*>, char*>(reinterpret_cast<char*>(dma->fAddress));

		if (!dmaOwnPtr)
			return {};

		kout << "Returning the new OwnPtr<IOBuf<Char*>>!\r";
		return dmaOwnPtr;
	}

	DMAWrapper& DMAWrapper::operator=(voidPtr Ptr)
	{
		this->fAddress = Ptr;
		return *this;
	}
} // namespace Kernel
