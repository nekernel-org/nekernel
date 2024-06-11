/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <HALKit/AMD64/HalPageAlloc.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/PageManager.hpp>

namespace NewOS
{
	namespace Detail
	{
		VoidPtr create_page_wrapper(Boolean rw, Boolean user, SizeT pageSz);
		void	exec_disable(UIntPtr addr);
		bool	page_disable(UIntPtr addr);
	} // namespace Detail
} // namespace NewOS
