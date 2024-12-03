/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

	File: HalPagingMgr.cc
	Purpose: Platform Paging Manager..

------------------------------------------- */

#include <HALKit/ARM64/Paging.h>
#include <HALKit/ARM64/Processor.h>

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

		static ZKA_PAGE_STORE& The()
		{
			static ZKA_PAGE_STORE the;
			return the;
		}
	};

	/// \brief Retrieve the page status of a PTE.
	STATIC Void mmi_page_status(PTE* pte)
	{
	}

	STATIC Int32 mmi_map_page_table_entry(VoidPtr virtual_address, UInt32 flags, PTE* pt_entry);

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

		ZKA_PAGE_STORE& page_store = ZKA_PAGE_STORE::The();

		while (page_store.fStoreOp)
			;

		page_store.fStoreOp = Yes;

		if (page_store.fInternalStore.fVAddr == virtual_address)
		{
			page_store.fStoreOp = No;
			return mmi_map_page_table_entry(page_store.fInternalStore.fVAddr, flags, page_store.fInternalStore.fPte);
		}

		return 1;
	}

	/// @brief Maps flags for a specific pte.
	/// @internal Internal function.
	STATIC Int32 mmi_map_page_table_entry(VoidPtr virtual_address, UInt32 flags, PTE* pt_entry)
	{
		ZKA_PAGE_STORE& page_store = ZKA_PAGE_STORE::The();

		// Update Internal store.

		page_store.fInternalStore.fPde	 = nullptr;
		page_store.fInternalStore.fPte	 = pt_entry;
		page_store.fInternalStore.fVAddr = virtual_address;

		page_store.fStoreOp = No;

		return 0;
	}
} // namespace Kernel::HAL
