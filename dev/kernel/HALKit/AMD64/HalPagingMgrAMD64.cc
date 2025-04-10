/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

	File: HalPagingMgr.cc
	Purpose: Platform Paging Manager.

------------------------------------------- */

#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Processor.h>

namespace Kernel::HAL
{
	/***********************************************************************************/
	/// @brief Gets a physical address from a virtual address.
	/// @param virt a valid virtual address.
	/// @return Physical address.
	/***********************************************************************************/
	UIntPtr hal_get_phys_address(VoidPtr virt)
	{
		const UInt64 vaddr			 = (UInt64)virt;
		const UInt64 kMask9Bits		 = 0x1FFULL;
		const UInt64 kPageOffsetMask = 0xFFFULL;

		UInt64 cr3 = (UInt64)hal_read_cr3() & ~kPageOffsetMask;

		// Level 4
		auto   pml4	 = reinterpret_cast<UInt64*>(cr3);
		UInt64 pml4e = pml4[(vaddr >> 39) & kMask9Bits];

		if (!(pml4e & 1))
			return 0;

		// Level 3
		auto   pdpt	 = reinterpret_cast<UInt64*>(pml4e & ~kPageOffsetMask);
		UInt64 pdpte = pdpt[(vaddr >> 30) & kMask9Bits];

		if (!(pdpte & 1))
			return 0;

		// Level 2
		auto   pd  = reinterpret_cast<UInt64*>(pdpte & ~kPageOffsetMask);
		UInt64 pde = pd[(vaddr >> 21) & kMask9Bits];

		if (!(pde & 1))
			return 0;

		// 1 GiB page support
		if (pde & (1 << 7))
		{
			return (pde & ~((1ULL << 30) - 1)) | (vaddr & ((1ULL << 30) - 1));
		}

		// Level 1
		auto   pt  = reinterpret_cast<UInt64*>(pde & ~kPageOffsetMask);
		UInt64 pte = pt[(vaddr >> 12) & kMask9Bits];

		if (!(pte & 1))
			return 0;

		return (pte & ~kPageOffsetMask) | (vaddr & kPageOffsetMask);
	}

	/***********************************************************************************/
	/// \brief Retrieve the page status of a PTE.
	/// \param pte Page Table Entry pointer.
	/***********************************************************************************/
	STATIC Void mmi_page_status(PTE* pte)
	{
		kout << (pte->Present ? "Present" : "Not Present") << kendl;
		kout << (pte->Wr ? "W/R" : "Not W/R") << kendl;
		kout << (pte->ExecDisable ? "NX" : "Not NX") << kendl;
		kout << (pte->User ? "User" : "Not User") << kendl;
	}

	STATIC Int32 mmi_map_page_table_entry(UIntPtr virtual_address, UInt32 physical_address, UInt32 flags, NE_PTE* pt_entry, NE_PDE* pd_entry);

	/***********************************************************************************/
	/// @brief Maps or allocates a page from virtual_address.
	/// @param virtual_address a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manipulation process.
	/***********************************************************************************/
	EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags)
	{
		const UInt64	 vaddr	   = (UInt64)virtual_address;
		constexpr UInt64 kMask9	   = 0x1FF;
		constexpr UInt64 kPageMask = 0xFFF;

		UInt64 cr3 = (UIntPtr)hal_read_cr3() & ~kPageMask;

		auto   pml4	 = reinterpret_cast<UInt64*>(cr3);
		UInt64 pml4e = pml4[(vaddr >> 39) & kMask9];

		if (!(pml4e & 1))
			return 1;

		auto   pdpt	 = reinterpret_cast<UInt64*>(pml4e & ~kPageMask);
		UInt64 pdpte = pdpt[(vaddr >> 30) & kMask9];

		if (!(pdpte & 1))
			return 1;

		auto   pd  = reinterpret_cast<UInt64*>(pdpte & ~kPageMask);
		UInt64 pde = pd[(vaddr >> 21) & kMask9];

		if (!(pde & 1))
			return 1;

		auto	pt	= reinterpret_cast<UInt64*>(pde & ~kPageMask);
		UInt64& pte = pt[(vaddr >> 12) & kMask9];

		// Set the new PTE
		pte = (reinterpret_cast<UInt64>(physical_address) & ~0xFFFULL) | 0x01ULL; // Present

		if (flags & ~kMMFlagsPresent)
			pte &= ~(0x01ULL); // Not Present

		if (flags & kMMFlagsWr)
			pte |= 1 << 1; // Writable

		if (flags & kMMFlagsUser)
			pte |= 1 << 2; // User

		if (flags & kMMFlagsNX)
			pte |= 1ULL << 63; // NX

		hal_invl_tlb(virtual_address);
		return 0;
	}
} // namespace Kernel::HAL
