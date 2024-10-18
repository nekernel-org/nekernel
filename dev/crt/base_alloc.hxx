/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <crt/defines.hxx>

namespace std::base_alloc
{
	/// @brief allocate a new class.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass, typename... Args>
	inline KindClass* allocate(Args&&... args)
	{
		return new KindClass(forward(args)...);
	}

	/// @brief allocate a new class.
	/// @note aborts on error.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass, typename... Args>
	inline KindClass* allocate_nothrow(Args&&... args) noexcept
	{
		return allocate(forward(args)...);
	}

	/// @brief free a class.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass>
	inline void release(KindClass ptr)
	{
		if (!ptr)
			return;

		delete ptr;
	}

	/// @brief destroy and free a class.
	/// @note aborts on error.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass>
	inline void release_nothrow(KindClass ptr) noexcept
	{
		release(ptr);
	}
} // namespace std::base_alloc
