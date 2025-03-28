/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/PCI/DMA.h>

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

		kout << "[DMAWrapper::IsIn] Checking offset..\n";
		return reinterpret_cast<UIntPtr>(this->fAddress) >= offset;
	}

	bool DMAWrapper::Write(const UIntPtr& bit, const UInt32& offset)
	{
		kout << "[DMAWrapper::Read] Checking this->fAddress..\n";

		if (!this->fAddress)
			return false;

		kout << "[DMAWrapper::Write] Writing at address..\n";

		auto addr =
			(volatile UIntPtr*)(reinterpret_cast<UIntPtr>(this->fAddress) + offset);
		*addr = bit;

		return true;
	}

	UIntPtr DMAWrapper::Read(const UInt32& offset)
	{
		kout << "[DMAWrapper::Read] Checking this->fAddress..\n";

		if (!this->fAddress)
			return 0;

		kout << "[DMAWrapper::Read] Reading this->fAddress..\n";

		return *(volatile UIntPtr*)(reinterpret_cast<UIntPtr>(this->fAddress) + offset);
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

		kout << "Returning the new OwnPtr<IOBuf<Char*>>!\r";
		return dmaOwnPtr;
	}

	DMAWrapper& DMAWrapper::operator=(voidPtr Ptr)
	{
		this->fAddress = Ptr;
		return *this;
	}
} // namespace Kernel
