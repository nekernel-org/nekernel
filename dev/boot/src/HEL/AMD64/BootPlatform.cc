
/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>
#include <BootKit/BootKit.h>

#ifdef __BOOTZ_STANDALONE__

using namespace Boot;

EXTERN_C void rt_hlt()
{
	asm volatile("hlt");
}

EXTERN_C void rt_cli()
{
	asm volatile("cli");
}

EXTERN_C void rt_sti()
{
	asm volatile("sti");
}

EXTERN_C void rt_cld()
{
	asm volatile("cld");
}

EXTERN_C void rt_std()
{
	asm volatile("std");
}

#else

#include <HALKit/AMD64/Processor.h>

void rt_hlt()
{
	Kernel::HAL::rt_halt();
}

#endif // __BOOTZ_STANDALONE__
