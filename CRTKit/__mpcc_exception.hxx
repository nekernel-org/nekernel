/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

extern "C" int __exit(int code);
/// @brief Standard C++ namespace
namespace std
{
	class ofstream;

	extern ofstream cout;

	inline void __throw_general(void)
	{
		__exit(33);
	}

	inline void __throw_domain_error(const char* error)
	{
		cout << "ZKA C++: Domain error: " << error << "\r";
		__throw_general();
		__builtin_unreachable(); // prevent from continuing.
	}

	inline void __throw_bad_array_new_length(void)
	{
		cout << "ZKAC C++: Bad array length.\r";
		__throw_general();
		__builtin_unreachable(); // prevent from continuing.
	}
} // namespace std
