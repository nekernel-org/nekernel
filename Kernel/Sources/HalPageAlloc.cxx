/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>

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
	constexpr auto cVMTMagic = 0xDEEFD00D;

	namespace HAL
	{
		namespace Detail
		{
			struct VIRTUAL_MEMORY_HEADER
			{
				UInt32	Magic;
				Boolean Present;
				Boolean ReadWrite;
				Boolean User;
				SizeT	Size;
			};

			struct VirtualMemoryHeaderTraits final
			{
				/// @brief Get next header.
				/// @param current
				/// @return
				VIRTUAL_MEMORY_HEADER* Next(VIRTUAL_MEMORY_HEADER* current)
				{
					if (current->Magic != cVMTMagic)
						current->Size = 8196;

					return current + sizeof(VIRTUAL_MEMORY_HEADER) + current->Size;
				}

				/// @brief Get previous header.
				/// @param current
				/// @return
				VIRTUAL_MEMORY_HEADER* Prev(VIRTUAL_MEMORY_HEADER* current)
				{
					if (current->Magic != cVMTMagic)
						current->Size = 8196;

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

			///! fetch from the start.
			Detail::VIRTUAL_MEMORY_HEADER*	  vmHeader = reinterpret_cast<Detail::VIRTUAL_MEMORY_HEADER*>(kKernelVMTStart);
			Detail::VirtualMemoryHeaderTraits traits;

			while (vmHeader->Present &&
				   vmHeader->Magic == cVMTMagic)
			{
				vmHeader = traits.Next(vmHeader);
			}

			vmHeader->Magic		= cVMTMagic;
			vmHeader->Present	= true;
			vmHeader->ReadWrite = rw;
			vmHeader->User		= user;
			vmHeader->Size		= size;

			kAllocationInProgress = false;

			return reinterpret_cast<VoidPtr>(vmHeader + sizeof(Detail::VIRTUAL_MEMORY_HEADER));
		}

		/// @brief Allocate a new page to be used by the OS.
		/// @param rw read/write bit.
		/// @param user user bit.
		/// @return
		auto hal_alloc_page(Boolean rw, Boolean user, SizeT size) -> VoidPtr
		{
			/// Wait for a ongoing allocation to complete.
			while (kAllocationInProgress)
			{
				;
			}

			if (size == 0)
				++size;

			/// allocate new page.
			return hal_try_alloc_new_page(rw, user, size);
		}
	} // namespace HAL
} // namespace Kernel
