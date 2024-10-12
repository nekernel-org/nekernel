
/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

#ifdef __STANDALONE__

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

EXTERN_C void Out8(UInt16 port, UInt8 value)
{
	asm volatile("outb %%al, %1"
				 :
				 : "a"(value), "Nd"(port)
				 : "memory");
}

EXTERN_C void Out16(UInt16 port, UInt16 value)
{
	asm volatile("outw %%ax, %1"
				 :
				 : "a"(value), "Nd"(port)
				 : "memory");
}

EXTERN_C void Out32(UInt16 port, UInt32 value)
{
	asm volatile("outl %%eax, %1"
				 :
				 : "a"(value), "Nd"(port)
				 : "memory");
}

EXTERN_C UInt8 In8(UInt16 port)
{
	UInt8 value;
	asm volatile("inb %1, %%al"
				 : "=a"(value)
				 : "Nd"(port)
				 : "memory");

	return value;
}

EXTERN_C UInt16 In16(UInt16 port)
{
	UInt16 value;
	asm volatile("inw %%dx, %%ax"
				 : "=a"(value)
				 : "d"(port));

	return value;
}

EXTERN_C UInt32 In32(UInt16 port)
{
	UInt32 value;
	asm volatile("inl %1, %%eax"
				 : "=a"(value)
				 : "Nd"(port)
				 : "memory");

	return value;
}

#else

#include <HALKit/AMD64/Processor.hxx>

void rt_hlt()
{
	Kernel::HAL::rt_halt();
}

#endif // __STANDALONE__
