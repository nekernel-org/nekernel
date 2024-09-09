/* -------------------------------------------

	Copyright ZKA Technologies.

	File: HalProcessor.cxx
	Purpose: Platform processor routines.

------------------------------------------- */

#include <HALKit/AMD64/Processor.hxx>

/**
 * @file HalProcessorMgr.cxx
 * @brief CPU Processor managers.
 */

#define cPageSz		kPageSize				// 4KB pages
#define cTotalPgMem gib_cast(16)			// 16MB total memory
#define cTotalPages (cTotalPgMem / cPageSz) // Total number of pages
#define cBmpPgSz	(cTotalPages / 8)		// 1 bit per page in the bitmap

namespace Kernel::HAL
{
	namespace MM
	{
		UInt8 cPageBitMp[cBmpPgSz] = {0}; // Bitmap to track free/used pages

		Void pg_set_used(Int64 page_index)
		{
			cPageBitMp[page_index / 8] |= (1 << (page_index % 8));
		}

		Void pg_set_free(Int64 page_index)
		{
			cPageBitMp[page_index / 8] &= ~(1 << (page_index % 8));
		}

		Int32 pg_is_free(Int64 page_index)
		{
			return !(cPageBitMp[page_index / 8] & (1 << (page_index % 8)));
		}

		VoidPtr pg_allocate()
		{
			for (SizeT i = 0; i < cTotalPages; i++)
			{
				if (pg_is_free(i))
				{
					pg_set_used(i);
					kcout << "Page has been allocated at index: " << number(i) << endl;

					return (VoidPtr)(i * cPageSz); // Return physical address of the page
				}
			}

			return nullptr; // No free page found
		}

		Void pg_delete(void* addr)
		{
			Int64 page_index = (UIntPtr)addr / cPageSz;
			kcout << "Page has been freed at: " << number(page_index) << endl;

			pg_set_free(page_index);
		}

	} // namespace MM

	/// @brief Maps or allocates a page from virt_addr.
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manip.
	EXTERN_C Int32 mm_map_page(VoidPtr p_virt_addr, UInt32 flags)
	{
		rt_cli();

		const auto cPageMask = 0xFFFF;

		volatile UIntPtr* pml4_base = (volatile UIntPtr*)(hal_read_cr3());

		UIntPtr virt_addr = (UIntPtr)p_virt_addr;

		UInt16 pml4_index = (virt_addr >> 39) & 0x1FF;
		UInt16 pdpt_index = (virt_addr >> 30) & 0x1FF;
		UInt16 pd_index	  = (virt_addr >> 21) & 0x1FF;
		UInt16 pt_index	  = (virt_addr >> 12) & 0x1FF;
		UInt16 offset	  = virt_addr & 0xFFF;

		const auto cIndexAlign = kPageAlign;

		// Now get pml4_entry
		volatile UIntPtr* pml4_entry = (volatile UInt64*)((pml4_base[pml4_index + cIndexAlign]));

		if (!(*pml4_entry & eFlagsPresent))
		{
			auto pml_addr = MM::pg_allocate();
			*pml4_entry	  = (UIntPtr)pml_addr | eFlagsPresent | eFlagsRw;
		}

		volatile UIntPtr* pdpt_entry = (volatile UIntPtr*)(pml4_entry[pdpt_index + cIndexAlign]);

		if (!(*pdpt_entry & eFlagsPresent))
		{
			auto pdpt_addr = MM::pg_allocate();
			*pdpt_entry	   = (UIntPtr)pdpt_addr | eFlagsPresent | eFlagsRw;
		}

		volatile UIntPtr* pd_entry = (volatile UIntPtr*)(pdpt_entry[pd_index + cIndexAlign]);

		if (!(*pd_entry & eFlagsPresent))
		{
			auto pd_addr = MM::pg_allocate();
			*pd_entry	 = (UIntPtr)pd_addr | eFlagsPresent | eFlagsRw;
		}

		volatile UIntPtr* pt_entry = (volatile UIntPtr*)(pd_entry[pt_index + cIndexAlign]);

		if (!(*pt_entry & eFlagsPresent))
		{
			PTE* frame = (PTE*)pt_entry;

			MM::pg_delete((VoidPtr)frame->PhysicalAddress);

			auto pt_addr = MM::pg_allocate();
			*pt_entry	 = (UIntPtr)pt_addr | eFlagsPresent | flags;

			kcout << (frame->Present ? "Page Present." : "Page Not Present.") << endl;
			kcout << (frame->Rw ? "Page RW." : "Page Not RW.") << endl;
			kcout << (frame->User ? "Page User." : "Page Not User.") << endl;

			kcout << "Physical Address: " << number(frame->PhysicalAddress) << endl;
		}
		else
		{
			PTE* frame = (PTE*)pt_entry;

			*pt_entry = (UIntPtr)(frame->PhysicalAddress / cPageSz) | flags;

			kcout << (frame->Present ? "Page Present." : "Page Not Present.") << endl;
			kcout << (frame->Rw ? "Page RW." : "Page Not RW.") << endl;
			kcout << (frame->User ? "Page User." : "Page Not User.") << endl;

			kcout << "Physical Address: " << number(frame->PhysicalAddress) << endl;
		}

		hal_invl_tlb(p_virt_addr);

		rt_sti();
		return 0;
	}

	Void Out8(UInt16 port, UInt8 value)
	{
		asm volatile("outb %%al, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	Void Out16(UInt16 port, UInt16 value)
	{
		asm volatile("outw %%ax, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	Void Out32(UInt16 port, UInt32 value)
	{
		asm volatile("outl %%eax, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	UInt8 In8(UInt16 port)
	{
		UInt8 value = 0UL;
		asm volatile("inb %1, %%al"
					 : "=a"(value)
					 : "Nd"(port)
					 : "memory");

		return value;
	}

	UInt16 In16(UInt16 port)
	{
		UInt16 value = 0UL;
		asm volatile("inw %1, %%ax"
					 : "=a"(value)
					 : "Nd"(port)
					 : "memory");

		return value;
	}

	UInt32 In32(UInt16 port)
	{
		UInt32 value = 0UL;
		asm volatile("inl %1, %%eax"
					 : "=a"(value)
					 : "Nd"(port)
					 : "memory");

		return value;
	}

	Void rt_halt()
	{
		asm volatile("hlt");
	}

	Void rt_cli()
	{
		asm volatile("cli");
	}

	Void rt_sti()
	{
		asm volatile("sti");
	}

	Void rt_cld()
	{
		asm volatile("cld");
	}

	Void rt_std()
	{
		asm volatile("std");
	}
} // namespace Kernel::HAL
