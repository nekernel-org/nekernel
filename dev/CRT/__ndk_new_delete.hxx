/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <CRT/__ndk_defines.hxx>

namespace stdx
{
	/// @brief allocate a new class.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass, typename... Args>
	inline void* allocate(Args&&... args)
	{
		return new KindClass(forward(args)...);
	}

	/// @brief free a class.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass>
	inline void release(KindClass ptr)
	{
		delete ptr;
	}
} // namespace stdx
