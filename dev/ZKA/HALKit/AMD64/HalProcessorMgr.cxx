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

namespace Kernel::HAL
{
	/// @brief Set a PTE from pd_base.
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manip.
	EXTERN_C Int32 mm_map_page(VoidPtr virt_addr, VoidPtr phys_addr, UInt32 flags)
	{
		rt_cli();

		const auto cPAddrMask = 0x000ffffffffff000;
		const auto cFlagsMask = 0xFFF;

		VoidPtr pml4_base = hal_read_cr3();

		UIntPtr pd_idx	= ((UIntPtr)virt_addr >> 22);
		UIntPtr pte_idx = ((UIntPtr)virt_addr >> 12) & 0x03FF;

		// Now get pd_entry
		volatile UInt64* pd_entry = (volatile UInt64*)(((UInt64)pml4_base) + (pd_idx * kPTEAlign));

		UInt64 pt_base = *pd_entry & ~0xFFF; // Remove flags

		switch ((UIntPtr)phys_addr)
		{
		case kBadAddress: {
			phys_addr = (VoidPtr)((pt_base & cPAddrMask) + ((UIntPtr)virt_addr & cFlagsMask));
			break;
		}
		default: {
			break;
		}
		}

		// And then PTE
		volatile UIntPtr* pt_entry = (volatile UIntPtr*)(pt_base + (pte_idx * kPTEAlign));

		if (!(*pt_entry & eFlagsPresent))
		{
			*pt_entry = ((UIntPtr)phys_addr) | (flags & 0xFFF) | eFlagsPresent;

			hal_invl_tlb((VoidPtr)virt_addr);

			kcout << "=================================================\r";
			kcout << "Post page allocation.\r";
			kcout << "=================================================\r";

			kcout << (*pt_entry & eFlagsPresent ? "Page Present." : "Page Not Present.") << endl;
			kcout << (*pt_entry & eFlagsRw ? "Page RW." : "Page Not RW.") << endl;
			kcout << (*pt_entry & eFlagsUser ? "Page User." : "Page Not User.") << endl;

			rt_sti();
			return 0;
		}

		*pt_entry = ((UIntPtr)phys_addr) | (flags & 0xFFF) | eFlagsPresent;

		hal_invl_tlb((VoidPtr)virt_addr);

		kcout << "=================================================\r";
		kcout << "Post page change.\r";
		kcout << "=================================================\r";

		kcout << (*pt_entry & eFlagsPresent ? "Page Present." : "Page Not Present.") << endl;
		kcout << (*pt_entry & eFlagsRw ? "Page RW." : "Page Not RW.") << endl;
		kcout << (*pt_entry & eFlagsUser ? "Page User." : "Page Not User.") << endl;

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
