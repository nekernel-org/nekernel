/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/HError.hpp>
#include <KernelKit/KernelHeap.hpp>
#include <NewKit/Crc32.hpp>
#include <NewKit/PageManager.hpp>

//! @file KernelHeap.cxx
//! @brief Kernel allocator.

#define kKernelHeapMagic		   (0xD4D7D5)
#define kKernelHeapHeaderPaddingSz (16U)

namespace NewOS
{
	STATIC SizeT	   kHeapCount = 0UL;
	STATIC PageManager kHeapPageManager;

	namespace Detail
	{
		/// @brief Kernel heap information block.
		/// Located before the address bytes.
		/// | HIB |  ADDRESS  |
		struct PACKED HeapInformationBlock final
		{
			///! @brief 32-bit value which contains the magic number of the executable.
			UInt32 fMagic;
			///! @brief Boolean value which tells if the pointer is allocated.
			Boolean fPresent;
			///! @brief 32-bit CRC checksum
			UInt32 fCRC32;
			/// @brief 64-bit pointer size.
			SizeT fTargetPtrSize;
			/// @brief 64-bit target pointer.
			UIntPtr fTargetPtr;
			/// @brief Is this a page pointer?
			Boolean fPagePtr;
			/// @brief Padding bytes for header.
			UInt8 fPadding[kKernelHeapHeaderPaddingSz];
		};

		typedef HeapInformationBlock* HeapInformationBlockPtr;
	} // namespace Detail

	/// @brief allocate chunk of memory.
	/// @param sz size of pointer
	/// @param rw read write (true to enable it)
	/// @param user is it accesible by user processes?
	/// @return the pointer
	VoidPtr ke_new_ke_heap(SizeT sz, const bool rw, const bool user)
	{
		if (sz == 0)
			++sz;

		auto wrapper = kHeapPageManager.Request(rw, user, false, sz);

		Detail::HeapInformationBlockPtr heapInfo =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				wrapper.VirtualAddress());

		heapInfo->fTargetPtrSize = sz;
		heapInfo->fMagic		 = kKernelHeapMagic;
		heapInfo->fCRC32		 = 0; // dont fill it for now.
		heapInfo->fTargetPtr	 = wrapper.VirtualAddress();
		heapInfo->fPagePtr		 = 0;

		++kHeapCount;

		return reinterpret_cast<VoidPtr>(wrapper.VirtualAddress() +
										 sizeof(Detail::HeapInformationBlock));
	}

	/// @brief Makes a page heap.
	/// @param heapPtr
	/// @return
	Int32 ke_make_ke_page(VoidPtr heapPtr)
	{
		if (kHeapCount < 1)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - sizeof(Detail::HeapInformationBlock)) <= 0)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - kBadPtr) < 0)
			return -kErrorInternal;

		Detail::HeapInformationBlockPtr virtualAddress =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

		virtualAddress->fPagePtr = 1;

		return 0;
	}

	/// @brief Declare pointer as free.
	/// @param heapPtr the pointer.
	/// @return
	Int32 ke_delete_ke_heap(VoidPtr heapPtr)
	{
		if (kHeapCount < 1)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - sizeof(Detail::HeapInformationBlock)) <= 0)
			return -kErrorInternal;
		if (((IntPtr)heapPtr - kBadPtr) < 0)
			return -kErrorInternal;

		Detail::HeapInformationBlockPtr virtualAddress =
			reinterpret_cast<Detail::HeapInformationBlockPtr>(
				(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

		if (virtualAddress && virtualAddress->fMagic == kKernelHeapMagic)
		{
			if (!virtualAddress->fPresent)
			{
				return -kErrorHeapNotPresent;
			}

			if (virtualAddress->fCRC32 != 0)
			{
				if (virtualAddress->fCRC32 !=
					ke_calculate_crc32((Char*)virtualAddress->fTargetPtr,
									   virtualAddress->fTargetPtrSize))
				{
					ke_stop(RUNTIME_CHECK_POINTER);
				}
			}

			virtualAddress->fTargetPtrSize = 0UL;
			virtualAddress->fPresent	   = false;
			virtualAddress->fTargetPtr	   = 0;
			virtualAddress->fCRC32		   = 0;
			virtualAddress->fMagic		   = 0;

			PTEWrapper		 pageWrapper(false, false, false, (UIntPtr)virtualAddress);
			Ref<PTEWrapper*> pteAddress{&pageWrapper};

			kHeapPageManager.Free(pteAddress);

			--kHeapCount;
			return 0;
		}

		return -kErrorInternal;
	}

	/// @brief Check if pointer is a valid kernel pointer.
	/// @param heapPtr the pointer
	/// @return if it exists.
	Boolean ke_is_valid_heap(VoidPtr heapPtr)
	{
		if (kHeapCount < 1)
			return false;

		if (heapPtr)
		{
			Detail::HeapInformationBlockPtr virtualAddress =
				reinterpret_cast<Detail::HeapInformationBlockPtr>(
					(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

			if (virtualAddress->fPresent && virtualAddress->fMagic == kKernelHeapMagic)
			{
				return true;
			}
		}

		return false;
	}

	/// @brief Protect the heap with a CRC value.
	/// @param heapPtr HIB pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean ke_protect_ke_heap(VoidPtr heapPtr)
	{
		if (heapPtr)
		{
			Detail::HeapInformationBlockPtr virtualAddress =
				reinterpret_cast<Detail::HeapInformationBlockPtr>(
					(UIntPtr)heapPtr - sizeof(Detail::HeapInformationBlock));

			if (virtualAddress->fPresent && kKernelHeapMagic == virtualAddress->fMagic)
			{
				virtualAddress->fCRC32 =
					ke_calculate_crc32((Char*)virtualAddress->fTargetPtr, virtualAddress->fTargetPtrSize);

				return true;
			}
		}

		return false;
	}
} // namespace NewOS
