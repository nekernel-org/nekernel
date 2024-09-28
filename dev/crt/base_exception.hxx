/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <crt/defines.hxx>

/// @brief CRT exit, with exit code (!!! exits all threads. !!!)
/// @param code
/// @return
extern "C" int __exit(int code);

/// @brief Standard C++ namespace
namespace std
{
	inline void __throw_general(void)
	{
		__exit(33);
	}

	inline void __throw_domain_error(const char* error)
	{
		__throw_general();
		__builtin_unreachable(); // prevent from continuing.
	}

	inline void __throw_bad_array_new_length(void)
	{
		__throw_general();
		__builtin_unreachable(); // prevent from continuing.
	}
} // namespace std
