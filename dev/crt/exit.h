/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <crt/defines.h>

extern "C" int exit(int code);

/// @brief Standard C++ namespace
namespace std
{
	inline int exit(int code)
	{
		return exit(code);
	}
} // namespace std
