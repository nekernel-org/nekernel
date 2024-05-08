/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.hpp>

/// @brief Standard C++ namespace
namespace std
{
#ifdef __x86_64__
#ifdef __KERNEL__
	inline void __throw_general(void)
	{
		__asm__("int $0xD");
	}
#else
	inline void __throw_general(void)
	{
		__exit(33);
	}
#endif
#else
	inline void __throw_general(void)
	{
		__exit(33);
	}
#endif // if __x86_64__

	inline void __throw_domain_error(const char* error)
	{
		NewOS::kcout << "MPCC C++: Domain error: " << error << "\r";
		__throw_general();
		CANT_REACH(); // prevent from continuing.
	}
} // namespace std
