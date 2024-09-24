/* -------------------------------------------

	Copyright ZKA Technologies.

	File: HalProcessor.cxx
	Purpose: Platform processor routines.

------------------------------------------- */

#include <HALKit/AMD64/Paging.hxx>
#include <HALKit/AMD64/Processor.hxx>

namespace Kernel::HAL
{
	STATIC Void mm_map_page_status(PTE* pte)
	{
		if (!pte)
			return;

		kcout << (pte->Present ? "Present" : "Not Present") << endl;
		kcout << (pte->Wr ? "W/R" : "Not W/R") << endl;
		kcout << (pte->User ? "User" : "Not User") << endl;
	}

	/// @brief Maps or allocates a page from virt_addr.
	/// @internal
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manipulation process.
	EXTERN_C Int32 mm_map_page(VoidPtr virt_addr, UInt32 flags)
	{
		return 0;
	}

	/// @brief Maps or allocates a page from virt_addr.
	/// @internal
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manipulation process.
	EXTERN_C Int32 _mm_map_page(VoidPtr virt_addr, UInt32 flags, ZKA_PDE* pde, SizeT pte)
	{
		if (!virt_addr || !pde || !pte)
			return -1;

		if (pde->fEntries[pte]->Present)
		{
			rt_cli();

			if (flags & ~eFlagsPresent)
				pde->fEntries[pte]->Present = false;
			else if (flags & eFlagsPresent)
				pde->fEntries[pte]->Present = true;

			if (flags & eFlagsRw)
				pde->fEntries[pte]->Wr = true;
			else if (flags & ~eFlagsRw)
				pde->fEntries[pte]->Wr = false;

			if (flags & eFlagsUser)
				pde->fEntries[pte]->User = true;
			else if (flags & ~eFlagsUser)
				pde->fEntries[pte]->User = false;

			mm_map_page_status(pde->fEntries[pte]);

			rt_sti();

			return 0;
		}

		return -2;
	}
} // namespace Kernel::HAL
