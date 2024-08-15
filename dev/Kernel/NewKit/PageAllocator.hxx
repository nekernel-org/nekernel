/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/PageManager.hxx>

namespace Kernel
{
	namespace Detail
	{
		VoidPtr create_page_wrapper(Boolean rw, Boolean user, SizeT pageSz);
		void	exec_disable(UIntPtr addr);
		bool	page_disable(UIntPtr addr);
	} // namespace Detail
} // namespace Kernel
