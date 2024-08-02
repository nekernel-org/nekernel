/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>

namespace stdx
{
	/// @brief allocate a new class.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass, typename... Args>
	inline Kernel::VoidPtr allocate(Args&&... args)
	{
		return new KindClass(Kernel::forward(args)...);
	}

	/// @brief free a class.
	/// @tparam KindClass the class type to allocate.
	template <typename KindClass>
	inline Kernel::Void release(KindClass ptr)
	{
		if (!ptr)
			return;
		delete ptr;
	}
} // namespace stdx
