/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

	File: HalPagingMgr.cc
	Purpose: Platform Paging Manager.

------------------------------------------- */

#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Processor.h>

namespace Kernel::HAL
{
	typedef UInt32 PageTableIndex;

	/***********************************************************************************/
	/// \brief Page store type.
	/***********************************************************************************/
	struct NE_PAGE_STORE final
	{
		struct
		{
			PDE*	fPde{nullptr};
			PTE*	fPte{nullptr};
			VoidPtr fPAddr{nullptr};
		} fInternalStore;

		Bool fStoreOp{No}; // Store operation is in progress.

		bool IsValidPage(PTE* pte)
		{
			return pte && pte->Present;
		}

		bool IsWRPage(PTE* pte)
		{
			return pte && pte->Wr;
		}

		bool IsUserPage(PTE* pte)
		{
			return pte && pte->User;
		}

		static NE_PAGE_STORE& The()
		{
			static NE_PAGE_STORE the;
			return the;
		}
	};

	/// @brief Go over the Page structure and find the address of *virtual_address*

	UInt64 hal_get_phys_address(VoidPtr virtual_address)
	{
		// Constants for table index bits
		const UInt64 kPmlIndexMask = 0x1FFULL; // Mask for PML4, PDPT, PD, PT index (9 bits)
		const UInt64 kPtIndexMask  = 0xFFFULL; // Mask for page table index (12 bits)

		UInt64 cr3 = (UInt64)hal_read_cr3();

		NE_PAGE_STORE& page_store = NE_PAGE_STORE::The();

		// Extract the indices from the virtual address
		UInt64 pml4_index = ((UIntPtr)virtual_address >> 39) & kPmlIndexMask;
		UInt64 pdpt_index = ((UIntPtr)virtual_address >> 30) & kPmlIndexMask;
		UInt64 pd_index	  = ((UIntPtr)virtual_address >> 21) & kPmlIndexMask;
		UInt64 pt_index	  = ((UIntPtr)virtual_address >> 12) & kPmlIndexMask;

		page_store.fStoreOp = Yes;

		const auto kPmlEntrySize = 8;

		// Read the PML4 entry from memory
		UInt64 pml4_base  = cr3 & ~kPtIndexMask;					  // CR3 points to the PML4 table base, mask off lower bits
		UInt64 pml4_entry = (pml4_base + pml4_index * kPmlEntrySize); // Each entry is 8 bytes

		// Read the PDPT entry
		UInt64 pdpt_base  = pml4_entry & ~kPtIndexMask; // Get the PDPT base physical address
		UInt64 pdpt_entry = (pdpt_base + pdpt_index * kPmlEntrySize);

		// Read the PD entry
		UInt64 pd_base	= pdpt_entry & ~kPtIndexMask; // Get the Page Directory base physical address
		UInt64 pd_entry = (pd_base + pd_index * kPmlEntrySize);

		// Read the PT entry
		UInt64 pt_base	= pd_entry & ~kPtIndexMask; // Get the Page Table base physical address
		UInt64 pt_entry = (pt_base + pt_index * kPmlEntrySize);

		// Lastly, grab the pte entry.
		NE_PDE* pde_struct = reinterpret_cast<NE_PDE*>(pt_base);

		return pde_struct->fEntries[pt_entry]->PhysicalAddress;
	}

	/***********************************************************************************/
	/// \brief Retrieve the page status of a PTE.
	/// \param pte Page Table Entry pointer.
	/***********************************************************************************/
	STATIC Void mmi_page_status(PTE* pte)
	{
		kout << (pte->Present ? "Present" : "Not Present") << endl;
		kout << (pte->Wr ? "W/R" : "Not W/R") << endl;
		kout << (pte->ExecDisable ? "NX" : "Not NX") << endl;
		kout << (pte->User ? "User" : "Not User") << endl;
	}

	STATIC Int32 mmi_map_page_table_entry(UInt32 physical_address, UInt32 flags, NE_PTE* pt_entry, NE_PDE* pd_entry);

	/***********************************************************************************/
	/// @brief Maps or allocates a page from virtual_address.
	/// @param virtual_address a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manipulation process.
	/***********************************************************************************/
	EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, VoidPtr physical_address, UInt32 flags)
	{
		// Constants for table index bits
		const UInt64 kPmlIndexMask = 0x1FFULL; // Mask for PML4, PDPT, PD, PT index (9 bits)
		const UInt64 kPtIndexMask  = 0xFFFULL; // Mask for page table index (12 bits)

		UInt64 cr3 = (UInt64)hal_read_cr3();

		NE_PAGE_STORE& page_store = NE_PAGE_STORE::The();

		// Extract the indices from the virtual address
		UInt64 pml4_index = ((UIntPtr)virtual_address >> 39) & kPmlIndexMask;
		UInt64 pdpt_index = ((UIntPtr)virtual_address >> 30) & kPmlIndexMask;
		UInt64 pd_index	  = ((UIntPtr)virtual_address >> 21) & kPmlIndexMask;
		UInt64 pt_index	  = ((UIntPtr)virtual_address >> 12) & kPmlIndexMask;

		page_store.fStoreOp = Yes;

		const auto kPmlEntrySize = 8;

		// Read the PML4 entry from memory
		UInt64 pml4_base  = cr3 & ~kPtIndexMask;					  // CR3 points to the PML4 table base, mask off lower bits
		UInt64 pml4_entry = (pml4_base + pml4_index * kPmlEntrySize); // Each entry is 8 bytes

		// Read the PDPT entry
		UInt64 pdpt_base  = pml4_entry & ~kPtIndexMask; // Get the PDPT base physical address
		UInt64 pdpt_entry = (pdpt_base + pdpt_index * kPmlEntrySize);

		// Read the PD entry
		UInt64 pd_base	= pdpt_entry & ~kPtIndexMask; // Get the Page Directory base physical address
		UInt64 pd_entry = (pd_base + pd_index * kPmlEntrySize);

		// Read the PT entry
		UInt64 pt_base	= pd_entry & ~kPtIndexMask; // Get the Page Table base physical address
		UInt64 pt_entry = (pt_base + pt_index * kPmlEntrySize);

		// Lastly, grab the pte entry.
		NE_PDE* pde_struct = reinterpret_cast<NE_PDE*>(pt_base);

		return mmi_map_page_table_entry((UInt32)(UInt64)physical_address, flags, pde_struct->fEntries[pt_entry], pde_struct);
	}

	/***********************************************************************************/
	/// @brief Maps flags for a specific pte.
	/// @internal Internal function.
	/***********************************************************************************/
	STATIC Int32 mmi_map_page_table_entry(UInt32 physical_address, UInt32 flags, NE_PTE* pt_entry, NE_PDE* pd_entry)
	{
		if (!pt_entry)
			return 1;

		pt_entry->Present = true;

		if (flags & kMMFlagsWr)
			pt_entry->Wr = true;
		else if (flags & ~kMMFlagsWr)
			pt_entry->Wr = false;

		if (flags & kMMFlagsNX)
			pt_entry->ExecDisable = true;
		else if (flags & ~kMMFlagsNX)
			pt_entry->ExecDisable = false;

		if (flags & kMMFlagsUser)
			pt_entry->User = true;
		else if (flags & ~kMMFlagsUser)
			pt_entry->User = false;

		pt_entry->PhysicalAddress = physical_address;

		hal_invl_tlb(reinterpret_cast<VoidPtr>(pt_entry));

		mmi_page_status(pt_entry);

		NE_PAGE_STORE& page_store = NE_PAGE_STORE::The();

		// Update Internal store.

		page_store.fInternalStore.fPde	 = pd_entry;
		page_store.fInternalStore.fPte	 = pt_entry;
		page_store.fInternalStore.fPAddr = (VoidPtr)(UIntPtr)physical_address;

		page_store.fStoreOp = No;

		return 0;
	}
} // namespace Kernel::HAL
