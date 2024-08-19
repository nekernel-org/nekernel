/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/DebugOutput.hxx>
#include <KernelKit/LPC.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/Crc32.hxx>
#include <NewKit/PageManager.hxx>

//! @file KernelHeap.cxx
//! @brief Kernel heap allocator.

#define kKernelHeapMagic		   (0xD4D7D5)
#define kKernelHeapHeaderPaddingSz (16U)

namespace Kernel
{
	SizeT	   kHeapCount = 0UL;
	PageManager kHeapPageManager;
	Bool kOperationInProgress = No;

	namespace Detail
	{
		/// @brief Kernel heap information block.
		/// Located before the address bytes.
		/// | HIB |  ADDRESS  |
		struct PACKED HEAP_INFORMATION_BLOCK final
		{
			///! @brief 32-bit value which contains the magic number of the heap.
			UInt32 fMagic;
			///! @brief Boolean value which tells if the heap is allocated.
			Boolean fPresent;
			///! @brief 32-bit CRC checksum.
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

		typedef HEAP_INFORMATION_BLOCK* HEAP_INFORMATION_BLOCK_PTR;

		Void mm_alloc_init_timeout(Void) noexcept
		{
			kOperationInProgress = Yes;
		}

		Void mm_alloc_fini_timeout(Void) noexcept
		{
			kOperationInProgress = No;
		}
	} // namespace Detail

	/// @brief Declare a new size for allocatedPtr.
	/// @param allocatedPtr the pointer.
	/// @return
	voidPtr mm_realloc_ke_heap(voidPtr allocatedPtr, SizeT newSz)
	{
		if (!allocatedPtr || newSz < 1)
			return nullptr;

		Detail::HEAP_INFORMATION_BLOCK_PTR heapInfoBlk =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)allocatedPtr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		heapInfoBlk->fTargetPtrSize = newSz;

		if (heapInfoBlk->fCRC32 > 0)
		{
			MUST_PASS(mm_protect_ke_heap(allocatedPtr));
		}

		return allocatedPtr;
	}

	/// @brief allocate chunk of memory.
	/// @param sz size of pointer
	/// @param rw read write (true to enable it)
	/// @param user is it accesible by user processes?
	/// @return The newly allocated pointer.
	VoidPtr mm_new_ke_heap(const SizeT sz, const bool rw, const bool user)
	{
		Detail::mm_alloc_init_timeout();

		auto szFix = sz;

		if (szFix == 0)
			++szFix;

		auto wrapper = kHeapPageManager.Request(rw, user, false, szFix);

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				wrapper.VirtualAddress());

		heap_info_ptr->fTargetPtrSize = szFix;
		heap_info_ptr->fMagic		 = kKernelHeapMagic;
		heap_info_ptr->fCRC32		 = 0; // dont fill it for now.
		heap_info_ptr->fTargetPtr	 = wrapper.VirtualAddress();
		heap_info_ptr->fPagePtr		 = 0;

		++kHeapCount;

		Detail::mm_alloc_fini_timeout();

		return reinterpret_cast<VoidPtr>(wrapper.VirtualAddress() +
										 sizeof(Detail::HEAP_INFORMATION_BLOCK));
	}

	/// @brief Makes a page heap.
	/// @param heap_ptr
	/// @return
	Int32 mm_make_ke_page(VoidPtr heap_ptr)
	{
		if (kHeapCount < 1)
			return -kErrorInternal;
		if (((IntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK)) <= 0)
			return -kErrorInternal;
		if (((IntPtr)heap_ptr - kBadPtr) < 0)
			return -kErrorInternal;

		Detail::mm_alloc_init_timeout();

		Detail::HEAP_INFORMATION_BLOCK_PTR heapInfoBlk =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		heapInfoBlk->fPagePtr = 1;

		Detail::mm_alloc_fini_timeout();

		return 0;
	}

	/// @brief Declare pointer as free.
	/// @param heap_ptr the pointer.
	/// @return
	Int32 mm_delete_ke_heap(VoidPtr heap_ptr)
	{
		if (kHeapCount < 1)
			return -kErrorInternal;
		if (((IntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK)) <= 0)
			return -kErrorInternal;
		if (((IntPtr)heap_ptr - kBadPtr) < 0)
			return -kErrorInternal;

		Detail::mm_alloc_init_timeout();

		Detail::HEAP_INFORMATION_BLOCK_PTR heapInfoBlk =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		if (heapInfoBlk && heapInfoBlk->fMagic == kKernelHeapMagic)
		{
			if (!heapInfoBlk->fPresent)
			{
				Detail::mm_alloc_fini_timeout();
				return -kErrorHeapNotPresent;
			}

			if (heapInfoBlk->fCRC32 != 0)
			{
				if (heapInfoBlk->fCRC32 !=
					ke_calculate_crc32((Char*)heapInfoBlk->fTargetPtr,
									   heapInfoBlk->fTargetPtrSize))
				{
					ke_stop(RUNTIME_CHECK_POINTER);
				}
			}

			heapInfoBlk->fTargetPtrSize = 0UL;
			heapInfoBlk->fPresent		= false;
			heapInfoBlk->fTargetPtr		= 0;
			heapInfoBlk->fCRC32			= 0;
			heapInfoBlk->fMagic			= 0;

			PTEWrapper		 pageWrapper(false, false, false, reinterpret_cast<UIntPtr>(heapInfoBlk));
			Ref<PTEWrapper*> pteAddress{&pageWrapper};

			kHeapPageManager.Free(pteAddress);

			--kHeapCount;

			Detail::mm_alloc_fini_timeout();

			return 0;
		}

		return -kErrorInternal;
	}

	/// @brief Check if pointer is a valid kernel pointer.
	/// @param heap_ptr the pointer
	/// @return if it exists.
	Boolean mm_is_valid_heap(VoidPtr heap_ptr)
	{
		if (kHeapCount < 1)
			return false;

		if (heap_ptr)
		{
			Detail::HEAP_INFORMATION_BLOCK_PTR virtualAddress =
				reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
					(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

			if (virtualAddress->fPresent && virtualAddress->fMagic == kKernelHeapMagic)
			{
				return true;
			}
		}

		return false;
	}

	/// @brief Protect the heap with a CRC value.
	/// @param heap_ptr HIB pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean mm_protect_ke_heap(VoidPtr heap_ptr)
	{
		if (heap_ptr)
		{
			Detail::HEAP_INFORMATION_BLOCK_PTR heapInfoBlk =
				reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
					(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

			if (heapInfoBlk->fPresent && kKernelHeapMagic == heapInfoBlk->fMagic)
			{
				heapInfoBlk->fCRC32 =
					ke_calculate_crc32((Char*)heapInfoBlk->fTargetPtr, heapInfoBlk->fTargetPtrSize);

				return true;
			}
		}

		return false;
	}
} // namespace Kernel
