/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/DebugOutput.hxx>
#include <KernelKit/LPC.hxx>
#include <KernelKit/Heap.hxx>
#include <NewKit/Crc32.hxx>
#include <NewKit/PageMgr.hxx>
#include <NewKit/Utils.hxx>

//! @file KernelHeap.cxx
//! @brief Kernel heap allocator.

#define kKernelHeapMagic		   (0xD4D7D5)
#define kKernelHeapHeaderPaddingSz (__BIGGEST_ALIGNMENT__)

namespace Kernel
{

	/// @brief Contains data structures and algorithms for the heap.
	namespace Detail
	{
		struct PACKED HEAP_INFORMATION_BLOCK;

		/// @brief Kernel heap information block.
		/// Located before the address bytes.
		/// | HIB |  ADDRESS  |
		struct PACKED HEAP_INFORMATION_BLOCK final
		{
			///! @brief 32-bit value which contains the magic number of the heap.
			UInt32 fMagic;
			///! @brief Boolean value which tells if the heap is allocated.
			Boolean fPresent;
			/// @brief Is this valued owned by the user?
			Boolean fUser;
			/// @brief Is this a page pointer?
			Boolean fPage;
			///! @brief 32-bit CRC checksum.
			UInt32 fCRC32;
			/// @brief 64-bit pointer size.
			SizeT fHeapSize;
			/// @brief 64-bit target pointer.
			UIntPtr fHeapPtr;
			/// @brief Padding bytes for header.
			UInt8 fPadding[kKernelHeapHeaderPaddingSz];
		};

		/// @brief Check for heap address validity.
		/// @param heap_ptr The address_ptr
		/// @return Bool if the pointer is valid or not.
		auto mm_check_heap_address(VoidPtr heap_ptr) -> Bool
		{
			if (!heap_ptr)
				return false;

			/// Add that check in case we're having an integer underflow. ///

			auto base_heap = (IntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK);

			if (base_heap < 0)
			{
				return false;
			}

			return true;
		}

		typedef HEAP_INFORMATION_BLOCK* HEAP_INFORMATION_BLOCK_PTR;

		Void mm_alloc_init_timeout(Void) noexcept
		{
			ZKA_UNUSED(0);
		}

		Void mm_alloc_fini_timeout(Void) noexcept
		{
			ZKA_UNUSED(0);
		}
	} // namespace Detail

	Detail::HEAP_INFORMATION_BLOCK_PTR kLatestAllocation = nullptr;

	/// @brief Declare a new size for ptr_heap.
	/// @param ptr_heap the pointer.
	/// @return Newly allocated heap header.
	voidPtr mm_realloc_ke_heap(voidPtr ptr_heap, SizeT new_sz)
	{
		if (Detail::mm_check_heap_address(ptr_heap) == No)
			return nullptr;

		if (!ptr_heap || new_sz < 1)
			return nullptr;

		kcout << "This function is not implemented in the kernel, please refrain from using that.\r";
		ke_stop(RUNTIME_CHECK_PROCESS);

		return nullptr;
	}

	/// @brief Allocate chunk of memory.
	/// @param sz Size of pointer
	/// @param rw Read Write bit.
	/// @param user User enable bit.
	/// @return The newly allocated pointer.
	VoidPtr mm_new_ke_heap(const SizeT sz, const bool rw, const bool user)
	{
		Detail::mm_alloc_init_timeout();

		auto sz_fix = sz;

		if (sz_fix == 0)
			return nullptr;

		sz_fix += sizeof(Detail::HEAP_INFORMATION_BLOCK);

		PageMgr heap_mgr;
		auto	wrapper = heap_mgr.Request(rw, user, No, sz_fix);

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_info_ptr =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				wrapper.VirtualAddress() + sizeof(Detail::HEAP_INFORMATION_BLOCK));

		heap_info_ptr->fHeapSize = sz_fix;
		heap_info_ptr->fMagic	 = kKernelHeapMagic;
		heap_info_ptr->fCRC32	 = No; // dont fill it for now.
		heap_info_ptr->fHeapPtr	 = reinterpret_cast<UIntPtr>(heap_info_ptr) + sizeof(Detail::HEAP_INFORMATION_BLOCK);
		heap_info_ptr->fPage	 = No;
		heap_info_ptr->fUser	 = user;
		heap_info_ptr->fPresent	 = Yes;

		rt_set_memory(heap_info_ptr->fPadding, 0, kKernelHeapHeaderPaddingSz);

		auto result = reinterpret_cast<VoidPtr>(heap_info_ptr->fHeapPtr);

		kLatestAllocation = heap_info_ptr;

		Detail::mm_alloc_fini_timeout();

		return result;
	}

	/// @brief Makes a page heap.
	/// @param heap_ptr
	/// @return
	Int32 mm_make_ke_page(VoidPtr heap_ptr)
	{
		if (Detail::mm_check_heap_address(heap_ptr) == No)
			return -kErrorHeapNotPresent;

		Detail::mm_alloc_init_timeout();

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_blk =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)heap_ptr - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		if (!heap_ptr)
			return -kErrorHeapNotPresent;

		heap_blk->fPage = true;

		Detail::mm_alloc_fini_timeout();

		return kErrorSuccess;
	}

	/// @brief Declare pointer as free.
	/// @param heap_ptr the pointer.
	/// @return
	Int32 mm_delete_ke_heap(VoidPtr heap_ptr)
	{
		if (Detail::mm_check_heap_address(heap_ptr) == No)
			return -kErrorHeapNotPresent;

		Detail::mm_alloc_init_timeout();

		Detail::HEAP_INFORMATION_BLOCK_PTR heap_blk =
			reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
				(UIntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK));

		if (heap_blk && heap_blk->fMagic == kKernelHeapMagic)
		{
			if (!heap_blk->fPresent)
			{
				Detail::mm_alloc_fini_timeout();
				return -kErrorHeapNotPresent;
			}

			if (heap_blk->fCRC32 != 0)
			{
				if (heap_blk->fCRC32 !=
					ke_calculate_crc32((Char*)heap_blk->fHeapPtr,
									   heap_blk->fHeapSize))
				{
					if (!heap_blk->fUser)
					{
						ke_stop(RUNTIME_CHECK_POINTER);
					}
				}
			}

			heap_blk->fHeapSize = 0UL;
			heap_blk->fPresent	= No;
			heap_blk->fHeapPtr	= 0;
			heap_blk->fCRC32	= 0;
			heap_blk->fMagic	= 0;

			PTEWrapper		pageWrapper(false, false, false, reinterpret_cast<UIntPtr>(heap_blk) - sizeof(Detail::HEAP_INFORMATION_BLOCK));
			Ref<PTEWrapper> pteAddress{pageWrapper};

			kcout << "Freeing pointer address: " << hex_number(reinterpret_cast<UIntPtr>(heap_blk) - sizeof(Detail::HEAP_INFORMATION_BLOCK)) << endl;

			PageMgr heap_mgr;
			heap_mgr.Free(pteAddress);

			Detail::mm_alloc_fini_timeout();

			return kErrorSuccess;
		}

		return -kErrorInternal;
	}

	/// @brief Check if pointer is a valid Kernel pointer.
	/// @param heap_ptr the pointer
	/// @return if it exists.
	Boolean mm_is_valid_heap(VoidPtr heap_ptr)
	{
		if (heap_ptr)
		{
			Detail::HEAP_INFORMATION_BLOCK_PTR heap_blk =
				reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
					(UIntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK));

			if (heap_blk && heap_blk->fPresent && heap_blk->fMagic == kKernelHeapMagic)
			{
				return Yes;
			}
		}

		return No;
	}

	/// @brief Protect the heap with a CRC value.
	/// @param heap_ptr HIB pointer.
	/// @return if it valid: point has crc now., otherwise fail.
	Boolean mm_protect_ke_heap(VoidPtr heap_ptr)
	{
		if (heap_ptr)
		{
			Detail::HEAP_INFORMATION_BLOCK_PTR heap_blk =
				reinterpret_cast<Detail::HEAP_INFORMATION_BLOCK_PTR>(
					(UIntPtr)(heap_ptr) - sizeof(Detail::HEAP_INFORMATION_BLOCK));

			if (heap_ptr && heap_blk->fPresent && kKernelHeapMagic == heap_blk->fMagic)
			{
				heap_blk->fCRC32 =
					ke_calculate_crc32((Char*)heap_blk->fHeapPtr, heap_blk->fHeapSize);

				return Yes;
			}
		}

		return No;
	}
} // namespace Kernel
