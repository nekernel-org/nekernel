/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

extern "C" int exit(int code);

/// @brief Standard C++ namespace
namespace std
{
	inline int exit(int code)
	{
		exit(code);
	}
} // namespace std