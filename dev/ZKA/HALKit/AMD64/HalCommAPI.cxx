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
	/// @brief Maps or allocates a page from virt_addr.
	/// @param virt_addr a valid virtual address.
	/// @param phys_addr point to physical address.
	/// @param flags the flags to put on the page.
	/// @return Status code of page manip.
	EXTERN_C Int32 mm_map_page(VoidPtr virt_addr, UInt32 flags)
	{
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
