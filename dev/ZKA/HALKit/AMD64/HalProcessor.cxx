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
	EXTERN_C Int32 mm_update_pte(VoidPtr pd_base, VoidPtr phys_addr, VoidPtr virt_addr, UInt32 flags)
	{
		UIntPtr pte_idx = (UIntPtr)virt_addr >> 12;

		volatile PTE* pte = (volatile PTE*)((UIntPtr)pd_base + (kPTEAlign * pte_idx));

		if (pte)
		{
			if (flags & eFlagsPresent)
				pte->Present = flags & eFlagsPresent;

			if (flags & eFlagsRw)
				pte->Rw = flags & eFlagsRw;

			if (flags & eFlagsUser)
				pte->User = flags & eFlagsUser;

			if (flags & eFlagsExecDisable)
				pte->ExecDisable = flags & eFlagsExecDisable;

			return Yes;
		}

		return No;
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
