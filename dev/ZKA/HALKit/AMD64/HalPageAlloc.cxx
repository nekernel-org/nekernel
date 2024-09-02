/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

#define cVMHMagic	(0xDEEFD00D)
#define cPaddingVMH (16)

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hxx>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/HalPageAlloc.hxx>
#endif

#include <NewKit/Defines.hxx>
#include <NewKit/KernelCheck.hxx>

Kernel::Boolean kAllocationInProgress = false;

namespace Kernel
{
	namespace HAL
	{
		namespace Detail
		{
			struct VIRTUAL_MEMORY_HEADER
			{
				UInt32	Magic;
				Boolean Present : 1;
				Boolean ReadWrite : 1;
				Boolean User : 1;
				SizeT	Size;
			};

			struct VirtualMemoryHeaderTraits final
			{
				/// @brief Get next header.
				/// @param current
				/// @return
				VIRTUAL_MEMORY_HEADER* Next(VIRTUAL_MEMORY_HEADER* current)
				{
					if (current->Magic != cVMHMagic)
						return current;

					return current + sizeof(VIRTUAL_MEMORY_HEADER) + current->Size;
				}

				/// @brief Get previous header.
				/// @param current
				/// @return
				VIRTUAL_MEMORY_HEADER* Prev(VIRTUAL_MEMORY_HEADER* current)
				{
					if (current->Magic != cVMHMagic)
						return current;

					return current - sizeof(VIRTUAL_MEMORY_HEADER) - current->Size;
				}
			};
		} // namespace Detail

		/// @brief Allocates a new page of memory.
		/// @param sz the size of it.
		/// @param rw read/write flag.
		/// @param user user flag.
		/// @return the page table of it.
		STATIC auto hal_try_alloc_new_page(Boolean rw, Boolean user, SizeT size) -> VoidPtr
		{
			if (kAllocationInProgress)
				return nullptr;

			kAllocationInProgress = true;

			//! fetch from the start.
			Detail::VIRTUAL_MEMORY_HEADER*	  vmh_header = reinterpret_cast<Detail::VIRTUAL_MEMORY_HEADER*>(kKernelVMHStart);
			Detail::VirtualMemoryHeaderTraits traits;

			while (vmh_header->Present &&
				   vmh_header->Magic == cVMHMagic)
			{
				vmh_header = traits.Next(vmh_header);

				if (vmh_header == reinterpret_cast<VoidPtr>(kBadPtr))
				{
					ke_stop(RUNTIME_CHECK_POINTER);
					return nullptr;
				}
			}

			vmh_header->Magic	  = cVMHMagic;
			vmh_header->Present	  = true;
			vmh_header->ReadWrite = rw;
			vmh_header->User	  = user;
			vmh_header->Size	  = size;

			kAllocationInProgress = false;

			auto result = reinterpret_cast<VoidPtr>(vmh_header + sizeof(Detail::VIRTUAL_MEMORY_HEADER));

			VoidPtr cr3 = hal_read_cr3();

			mm_update_page(cr3, 0, result, eFlagsPresent | (rw ? eFlagsRw : 0) | (user ? eFlagsUser : 0));

			return result;
		}

		/// @brief Allocate a new page to be used by the OS.
		/// @param rw read/write bit.
		/// @param user user bit.
		/// @return
		auto hal_alloc_page(Boolean rw, Boolean user, SizeT size) -> VoidPtr
		{
			kcout << "PageAlloc: Waiting now...";

			// Wait for a ongoing allocation to complete.
			while (kAllocationInProgress)
			{
				(Void)0;
			}

			kcout << ", done waiting, allocating...\r";

			if (size == 0)
				++size;

			// Now allocate the page.
			return hal_try_alloc_new_page(rw, user, size);
		}

		auto hal_free_page(VoidPtr page_ptr) -> Bool
		{
			if (!page_ptr)
				return false;

			Detail::VIRTUAL_MEMORY_HEADER* result = reinterpret_cast<Detail::VIRTUAL_MEMORY_HEADER*>((UIntPtr)page_ptr - sizeof(Detail::VIRTUAL_MEMORY_HEADER));

			if (result->Magic != cVMHMagic)
				return false;

			if (result->Present != true)
				return true;

			result->Present = false;

			return true;
		}
	} // namespace HAL
} // namespace Kernel
