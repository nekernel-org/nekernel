/* -------------------------------------------

	Copyright ZKA Technologies.

	File: HalPagingMgr.cxx
	Purpose: Platform Paging Manager..

------------------------------------------- */

#include <HALKit/AMD64/Paging.hxx>
#include <HALKit/AMD64/Processor.hxx>

namespace Kernel::HAL
{
	typedef UInt32 PageTableIndex;

	/// \brief Page store type.
	struct ZKA_PAGE_STORE final
	{
		struct
		{
			PDE*	fPde{nullptr};
			PTE*	fPte{nullptr};
			VoidPtr fVAddr{nullptr};
		} fInternalStore;

		Bool fStoreOp{No}; // Store operation in progress.

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

		static ZKA_PAGE_STORE& The()
		{
			static ZKA_PAGE_STORE the;
			return the;
		}
	};

	/// \brief Retrieve the page status of a PTE.
	STATIC Void mmi_page_status(PTE* pte)
	{
		kcout << (pte->Present ? "Present" : "Not Present") << endl;
		kcout << (pte->Wr ? "W/R" : "Not W/R") << endl;
		kcout << (pte->ExecDisable ? "NX" : "Not NX") << endl;
		kcout << (pte->User ? "User" : "Not User") << endl;
	}

	STATIC Int32 mmi_map_page_table_entry(VoidPtr virtual_address, UInt32 flags, ZKA_PTE* pt_entry);

	/// @brief Maps or allocates a page from virtual_address.
	/// @param virtual_address a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manipulation process.
	EXTERN_C Int32 mm_map_page(VoidPtr virtual_address, UInt32 flags)
	{
		if (!virtual_address ||
			!flags)
			return 0;

		// Constants for table index bits
		const UInt64 cPmlIndexMask = 0x1FFULL; // Mask for PML4, PDPT, PD, PT index (9 bits)
		const UInt64 cPtIndexMask  = 0xFFFULL; // Mask for page table index (12 bits)

		UInt64 cr3 = (UInt64)hal_read_cr3();

		ZKA_PAGE_STORE& page_store = ZKA_PAGE_STORE::The();

		// Extract the indices from the virtual address
		UInt64 pml4_index = ((UIntPtr)virtual_address >> 39) & cPmlIndexMask;
		UInt64 pdpt_index = ((UIntPtr)virtual_address >> 30) & cPmlIndexMask;
		UInt64 pd_index	  = ((UIntPtr)virtual_address >> 21) & cPmlIndexMask;
		UInt64 pt_index	  = ((UIntPtr)virtual_address >> 12) & cPmlIndexMask;

		while (page_store.fStoreOp)
			;

		page_store.fStoreOp = Yes;

		if (page_store.fInternalStore.fVAddr == virtual_address)
		{
			page_store.fStoreOp = No;
			return mmi_map_page_table_entry(page_store.fInternalStore.fVAddr, flags, page_store.fInternalStore.fPte);
		}

		const auto cPmlEntrySize = 8;

		// Read the PML4 entry from memory
		UInt64 pml4_base  = cr3 & ~cPtIndexMask;					  // CR3 points to the PML4 table base, mask off lower bits
		UInt64 pml4_entry = (pml4_base + pml4_index * cPmlEntrySize); // Each entry is 8 bytes

		// Read the PDPT entry
		UInt64 pdpt_base  = pml4_entry & ~cPtIndexMask; // Get the PDPT base physical address
		UInt64 pdpt_entry = (pdpt_base + pdpt_index * cPmlEntrySize);

		// Read the PD entry
		UInt64 pd_base	= pdpt_entry & ~cPtIndexMask; // Get the Page Directory base physical address
		UInt64 pd_entry = (pd_base + pd_index * cPmlEntrySize);

		// Read the PT entry
		UInt64 pt_base	= pd_entry & ~cPtIndexMask; // Get the Page Table base physical address
		UInt64 pt_entry = (pt_base + pt_index * cPmlEntrySize);

		// Lastly, grab the pte entry.
		ZKA_PDE* pte_struct = reinterpret_cast<ZKA_PDE*>(pt_base);

		return mmi_map_page_table_entry(virtual_address, flags, pte_struct->fEntries[pt_entry]);
	}

	/// @brief Maps flags for a specific pte.
	/// @internal Internal function.
	STATIC Int32 mmi_map_page_table_entry(VoidPtr virtual_address, UInt32 flags, ZKA_PTE* pt_entry)
	{
		if (flags & ~eFlagsPresent)
			pt_entry->Present = false;
		else if (flags & eFlagsPresent)
			pt_entry->Present = true;

		if (flags & eFlagsWr)
			pt_entry->Wr = true;
		else if (flags & ~eFlagsWr)
			pt_entry->Wr = false;

		if (flags & eFlagsNX)
			pt_entry->ExecDisable = true;
		else if (flags & ~eFlagsNX)
			pt_entry->ExecDisable = false;

		if (flags & eFlagsUser)
			pt_entry->User = true;
		else if (flags & ~eFlagsUser)
			pt_entry->User = false;

		hal_invl_tlb(reinterpret_cast<VoidPtr>(pt_entry));

		mmi_page_status(pt_entry);

		ZKA_PAGE_STORE& page_store = ZKA_PAGE_STORE::The();

		// Update Internal store.

		page_store.fInternalStore.fPde	 = nullptr;
		page_store.fInternalStore.fPte	 = pt_entry;
		page_store.fInternalStore.fVAddr = virtual_address;

		page_store.fStoreOp = No;

		return 0;
	}
} // namespace Kernel::HAL
