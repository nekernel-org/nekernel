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
	EXTERN_C Int32 mm_update_pte(VoidPtr virt_addr, UInt32 flags)
	{
		VoidPtr pml4_base = hal_read_cr3();

		UIntPtr pml4_idx = ((UIntPtr)virt_addr >> 39) & 0x1FFF;
		UIntPtr pdpt_idx = ((UIntPtr)virt_addr >> 30) & 0x1FFF;
		UIntPtr pd_idx	 = ((UIntPtr)virt_addr >> 21) & 0x1FFF;
		UIntPtr pte_idx	 = ((UIntPtr)virt_addr >> 12) & 0x1FFF;

		// Access PML4 entry
		volatile UInt64* pml4_entry = (volatile UInt64*)(((UInt64)pml4_base) + pml4_idx * sizeof(UIntPtr));
		UInt64			 pdpt_base	= *pml4_entry & ~0xFFF; // Remove flags (assuming 4KB pages)

		// Access PDPT entry
		volatile UInt64* pdpt_entry = (volatile UInt64*)(((UInt64)pdpt_base) + pdpt_idx * sizeof(UIntPtr));
		UInt64			 pd_base	= *pdpt_entry & ~0xFFF; // Remove flags

		// Now PD
		volatile UInt64* pd_entry = (volatile UInt64*)(((UInt64)pd_base) + pd_idx * sizeof(UIntPtr));
		UInt64			 pt_base  = *pd_entry & ~0xFFF; // Remove flags

		// And then PTE
		volatile UInt64* page_addr = (volatile UInt64*)(((UInt64)pt_base) + (pte_idx * sizeof(UIntPtr)));

		if (flags & eFlagsPresent)
			*page_addr |= 0x01; // present bit
		else if (flags & ~eFlagsPresent)
			*page_addr &= 0x01; // present bit

		if (flags & eFlagsRw)
			*page_addr |= 0x02;
		else if (flags & ~eFlagsRw)
			*page_addr &= 0x02; // present bit

		if (flags & eFlagsUser)
			*page_addr |= 0x04;
		else if (flags & ~eFlagsUser)
			*page_addr &= 0x04; // present bit

		hal_write_cr3((UIntPtr)pml4_base);

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
