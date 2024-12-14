
/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/PageMgr.h>
#include <NewKit/Ref.h>

namespace Kernel
{
	class Pmm;
	class PTEWrapper;

	class Pmm final
	{
	public:
		explicit Pmm();
		~Pmm();

		Pmm& operator=(const Pmm&) = delete;
		Pmm(const Pmm&)			   = default;

		Ref<PTEWrapper> RequestPage(Boolean user = false, Boolean readWrite = false);
		Boolean			FreePage(Ref<PTEWrapper> refPage);

		Boolean ToggleRw(Ref<PTEWrapper> refPage, Boolean enable = true);
		Boolean TogglePresent(Ref<PTEWrapper> refPage, Boolean enable = true);
		Boolean ToggleUser(Ref<PTEWrapper> refPage, Boolean enable = true);
		Boolean ToggleShare(Ref<PTEWrapper> refPage, Boolean enable = true);

		/// @brief Get the page manager of this.
		Ref<PageMgr>& Leak()
		{
			return fPageMgr;
		}

	private:
		Ref<PageMgr> fPageMgr;
	};
} // namespace Kernel
