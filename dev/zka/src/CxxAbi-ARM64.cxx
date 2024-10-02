/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifdef __ZKA_ARM64__

#include <KernelKit/DebugOutput.hxx>
#include <NewKit/CxxAbi.hxx>
#include <KernelKit/LPC.hxx>

EXTERN_C
{
#include <limits.h>
}

int const cUninitialized	= 0;
int const cBeingInitialized = -1;
int const cEpochStart		= INT_MIN;

EXTERN_C
{
	int			 _Init_global_epoch = cEpochStart;
	__thread int _Init_thread_epoch = cEpochStart;
}

Kernel::UInt32 const cNKTimeout = 100; // ms

EXTERN_C void __cdecl _Init_thread_wait(Kernel::UInt32 const timeout)
{
	MUST_PASS(timeout != INT_MAX);
}

EXTERN_C void __cdecl _Init_thread_header(int* const pOnce) noexcept
{
	if (*pOnce == cUninitialized)
	{
		*pOnce = cBeingInitialized;
	}
	else
	{
		while (*pOnce == cBeingInitialized)
		{
			_Init_thread_wait(cNKTimeout);

			if (*pOnce == cUninitialized)
			{
				*pOnce = cBeingInitialized;
				return;
			}
		}
		_Init_thread_epoch = _Init_global_epoch;
	}
}

EXTERN_C void __cdecl _Init_thread_abort(int* const pOnce) noexcept
{
	*pOnce = cUninitialized;
}

EXTERN_C void __cdecl _Init_thread_footer(int* const pOnce) noexcept
{
	++_Init_global_epoch;
	*pOnce			   = _Init_global_epoch;
	_Init_thread_epoch = _Init_global_epoch;
}

EXTERN_C void _purecall()
{
	ZKA_UNUSED(0);
}

#endif // ifdef __ZKA_ARM64__
