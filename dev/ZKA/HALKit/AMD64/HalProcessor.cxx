/* -------------------------------------------

	Copyright ZKA Technologies.

	File: HalProcessor.cxx
	Purpose: Platform processor routines.

------------------------------------------- */

#include <HALKit/AMD64/Processor.hxx>

/**
 * @file Processor.cpp
 * @brief This file is about processor specific functions (in/out/cli/std...)
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
		VoidPtr pml4_base = hal_read_cr3();

		UIntPtr pd_idx	= ((UIntPtr)virt_addr >> 22);
		UIntPtr pte_idx = ((UIntPtr)virt_addr >> 12) & 0x03FF;

		// Now get pd_entry
		volatile UInt64* pd_entry = (volatile UInt64*)(((UInt64)pml4_base) + (pd_idx * kPTEAlign));

		// Don't bother allocate directory.
		if (!(*pd_entry & 0x01))
		{
			ke_stop(RUNTIME_CHECK_PAGE);
		}

		UInt64 pt_base = pd_entry[pd_idx] & ~0xFFF; // Remove flags

		// And then PTE
		volatile UIntPtr* pt_entry = (volatile UIntPtr*)(pt_base + (pte_idx * kPTEAlign));

		kcout << (pt_entry[pte_idx] & 0x01 ? "Page Present." : "Page Not Present.") << endl;
		kcout << (pt_entry[pte_idx] & 0x02 ? "Page RW." : "Page Not RW.") << endl;
		kcout << (pt_entry[pte_idx] & 0x04 ? "Page User." : "Page Not User.") << endl;

		switch ((UIntPtr)phys_addr)
		{
		case kBadAddress: {
			phys_addr = (VoidPtr)((pt_entry[pte_idx] & ~0xFFF) + ((UIntPtr)virt_addr & 0xFFF));
			break;
		}
		default: {
			break;
		}
		}

		pt_entry[pte_idx] = ((UIntPtr)phys_addr) | (flags & 0xFFF) | 0x01;

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
