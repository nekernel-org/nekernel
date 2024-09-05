/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/PageMgr.hxx>

namespace Kernel
{
	namespace Detail
	{
		void	exec_disable(UIntPtr addr);
		bool	page_disable(UIntPtr addr);
	} // namespace Detail
} // namespace Kernel
