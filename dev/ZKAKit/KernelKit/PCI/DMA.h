/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DeviceMgr.h>
#include <KernelKit/PCI/Device.h>
#include <NewKit/Array.h>
#include <NewKit/OwnPtr.h>
#include <NewKit/Ref.h>

namespace Kernel
{
	enum class DmaKind
	{
		PCI, // Bus mastering is required to be turned on. Basiaclly a request
		// control system. 64-Bit access depends on the PAE bit and the device
		// (if Double Address Cycle is available)
		ISA, // Four DMA channels 0-3; 8 bit transfers and only a megabyte of RAM.
		Invalid,
	};

	class DMAWrapper final
	{
	public:
		explicit DMAWrapper() = delete;

	public:
		explicit DMAWrapper(nullPtr) = delete;
		explicit DMAWrapper(voidPtr Ptr, DmaKind Kind = DmaKind::PCI)
			: fAddress(Ptr), fKind(Kind)
		{
		}

	public:
		DMAWrapper& operator=(voidPtr Ptr);

	public:
		DMAWrapper& operator=(const DMAWrapper&) = default;
		DMAWrapper(const DMAWrapper&)			 = default;

	public:
		~DMAWrapper() = default;

		template <class T>
		T* operator->();

		template <class T>
		T* Get(const UIntPtr off = 0);

	public:
			 operator bool();
		bool operator!();

	public:
		bool	Write(const UIntPtr& bit, const UIntPtr& offset);
		UIntPtr Read(const UIntPtr& offset);
		Boolean Check(UIntPtr offset) const;

	public:
		UIntPtr operator[](const UIntPtr& offset);

	private:
		voidPtr fAddress{nullptr};
		DmaKind fKind{DmaKind::Invalid};

	private:
		friend class DMAFactory;
	};

	class DMAFactory final
	{
	public:
		static OwnPtr<IOBuf<Char*>> Construct(OwnPtr<DMAWrapper>& dma);
	};
} // namespace Kernel

#include <KernelKit/PCI/DMA.inl>
