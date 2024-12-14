/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __ZKA_AMD64__

#include <KernelKit/DebugOutput.h>
#include <NewKit/CxxAbi.h>
#include <KernelKit/LPC.h>

atexit_func_entry_t __atexit_funcs[kAtExitMacDestructors];

uarch_t __atexit_func_count;

/// @brief dynamic shared object Handle.
Kernel::UIntPtr __dso_handle;

EXTERN_C Kernel::Void __cxa_pure_virtual(void* self)
{
	kcout << "object: " << Kernel::number(reinterpret_cast<Kernel::UIntPtr>(self));
	kcout << ", has unimplemented virtual functions.\r";
}

EXTERN_C void ___chkstk_ms(void)
{
}

EXTERN_C int atexit(void (*f)(void*), void* arg, void* dso)
{
	if (__atexit_func_count >= kAtExitMacDestructors)
		return 1;

	__atexit_funcs[__atexit_func_count].destructor_func = f;
	__atexit_funcs[__atexit_func_count].obj_ptr			= arg;
	__atexit_funcs[__atexit_func_count].dso_handle		= dso;

	__atexit_func_count++;

	return 0;
}

EXTERN_C void __cxa_finalize(void* f)
{
	uarch_t i = __atexit_func_count;
	if (!f)
	{
		while (i--)
		{
			if (__atexit_funcs[i].destructor_func)
			{
				(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			};
		}

		return;
	}

	while (i--)
	{
		if (__atexit_funcs[i].destructor_func)
		{
			(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			__atexit_funcs[i].destructor_func = 0;
		};
	}
}

namespace cxxabiv1
{
	EXTERN_C int __cxa_guard_acquire(__guard* g)
	{
		(void)g;
		return 0;
	}

	EXTERN_C int __cxa_guard_release(__guard* g)
	{
		*(char*)g = 1;
		return 0;
	}

	EXTERN_C void __cxa_guard_abort(__guard* g)
	{
		(void)g;
	}
} // namespace cxxabiv1

#endif // ifdef __ZKA_AMD64__
