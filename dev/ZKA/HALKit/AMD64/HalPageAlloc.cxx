/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

#define cVMHMagic (0xDEEFD00D)
#define cPaddingVMH (512)

#ifdef __NEWOS_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hxx>
#elif defined(__NEWOS_ARM64__)
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
						current->Size = cPaddingVMH;

					return current + sizeof(VIRTUAL_MEMORY_HEADER) + current->Size;
				}

				/// @brief Get previous header.
				/// @param current
				/// @return
				VIRTUAL_MEMORY_HEADER* Prev(VIRTUAL_MEMORY_HEADER* current)
				{
					if (current->Magic != cVMHMagic)
						current->Size = cPaddingVMH;

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
			}

			vmh_header->Magic		= cVMHMagic;
			vmh_header->Present	= true;
			vmh_header->ReadWrite = rw;
			vmh_header->User		= user;
			vmh_header->Size		= size;

			kAllocationInProgress = false;

			return reinterpret_cast<VoidPtr>(vmh_header + sizeof(Detail::VIRTUAL_MEMORY_HEADER));
		}

		/// @brief Allocate a new page to be used by the OS.
		/// @param rw read/write bit.
		/// @param user user bit.
		/// @return
		auto hal_alloc_page(Boolean rw, Boolean user, SizeT size) -> VoidPtr
		{
			kcout << "Waiting now...";

			// Wait for a ongoing allocation to complete.
			while (kAllocationInProgress)
			{
				(void)0;
			}

			kcout << ", done waiting, allocating...\r";

			if (size == 0)
				++size;

			// allocate new page.
			return hal_try_alloc_new_page(rw, user, size);
		}
	} // namespace HAL
} // namespace Kernel
