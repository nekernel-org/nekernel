/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

	File: HalCPU.cc
	Purpose: Platform processor routines.

------------------------------------------- */

#include <HALKit/AMD64/Paging.h>
#include <HALKit/AMD64/Processor.h>

/**
 * @file HalCPU.cc
 * @brief Common CPU API.
 */

namespace Kernel::HAL
{
	Void lrt_out8(UInt16 port, UInt8 value)
	{
		asm volatile("outb %%al, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	Void lrt_out16(UInt16 port, UInt16 value)
	{
		asm volatile("outw %%ax, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	Void lrt_out32(UInt16 port, UInt32 value)
	{
		asm volatile("outl %%eax, %1"
					 :
					 : "a"(value), "Nd"(port)
					 : "memory");
	}

	UInt8 lrt_in8(UInt16 port)
	{
		UInt8 value = 0UL;
		asm volatile("inb %1, %%al"
					 : "=a"(value)
					 : "Nd"(port)
					 : "memory");

		return value;
	}

	UInt16 lrt_in16(UInt16 port)
	{
		UInt16 value = 0UL;
		asm volatile("inw %1, %%ax"
					 : "=a"(value)
					 : "Nd"(port)
					 : "memory");

		return value;
	}

	UInt32 lrt_in32(UInt16 port)
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
