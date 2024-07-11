/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <HALKit/AMD64/Processor.hxx>

/**
 * @file Processor.cpp
 * @brief This file is about processor specific functions (in/out/cli/std...)
 */

namespace Kernel::HAL
{
	void Out8(UInt16 port, UInt8 value)
	{
		asm volatile("outb %%al, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	void Out16(UInt16 port, UInt16 value)
	{
		asm volatile("outw %%ax, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	void Out32(UInt16 port, UInt32 value)
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

	void rt_halt()
	{
		asm volatile("hlt");
	}

	void rt_cli()
	{
		asm volatile("cli");
	}

	void rt_sti()
	{
		asm volatile("sti");
	}

	void rt_cld()
	{
		asm volatile("cld");
	}

	void rt_std()
	{
		asm volatile("std");
	}
} // namespace Kernel::HAL
