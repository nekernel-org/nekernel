/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/PCI/DMA.h>

namespace Kernel
{
	DMAWrapper::operator bool()
	{
		return fAddress;
	}

	bool DMAWrapper::operator!()
	{
		return !fAddress;
	}

	Boolean DMAWrapper::Check(UIntPtr offset) const
	{
		if (!fAddress)
			return false;
		if (offset == 0)
			return true;

		kcout << "[DMAWrapper::IsIn] Checking offset..\n";
		return reinterpret_cast<UIntPtr>(fAddress) >= offset;
	}

	bool DMAWrapper::Write(const UIntPtr& bit, const UIntPtr& offset)
	{
		if (!fAddress)
			return false;

		kcout << "[DMAWrapper::Write] Writing at address..\n";

		auto addr =
			(volatile UIntPtr*)(reinterpret_cast<UIntPtr>(fAddress) + offset);
		*addr = bit;

		return true;
	}

	UIntPtr DMAWrapper::Read(const UIntPtr& offset)
	{
		kcout << "[DMAWrapper::Read] checking fAddress..\n";
		if (!fAddress)
			return 0;

		kcout << "[DMAWrapper::Read] Reading fAddress..\n";
		return *(volatile UIntPtr*)(reinterpret_cast<UIntPtr>(fAddress) + offset);
		;
	}

	UIntPtr DMAWrapper::operator[](const UIntPtr& offset)
	{
		return this->Read(offset);
	}

	OwnPtr<IOBuf<Char*>> DMAFactory::Construct(OwnPtr<DMAWrapper>& dma)
	{
		if (!dma)
			return {};

		OwnPtr<IOBuf<Char*>> dmaOwnPtr =
			make_ptr<IOBuf<Char*>, char*>(reinterpret_cast<char*>(dma->fAddress));

		if (!dmaOwnPtr)
			return {};

		kcout << "Returning the new OwnPtr<IOBuf<Char*>>!\r";
		return dmaOwnPtr;
	}

	DMAWrapper& DMAWrapper::operator=(voidPtr Ptr)
	{
		fAddress = Ptr;
		return *this;
	}
} // namespace Kernel
