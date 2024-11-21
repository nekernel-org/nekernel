// a way to create and find our pages.
// I'm thinking about a separate way of getting a paged area.

/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <NewKit/Ref.h>

namespace Kernel
{
	class PageMgr;

	class PTEWrapper final
	{
	public:
		explicit PTEWrapper(Boolean Rw = false, Boolean User = false, Boolean ExecDisable = false, UIntPtr Address = 0);

		~PTEWrapper();

		PTEWrapper& operator=(const PTEWrapper&) = default;
		PTEWrapper(const PTEWrapper&)			 = default;

	public:
		const UIntPtr VirtualAddress();

		Void NoExecute(const bool enable = false);
		Bool NoExecute();

		operator bool()
		{
			return fVirtAddr;
		}

		bool Reclaim();
		bool Shareable();
		bool Present();
		bool Access();

	private:
		Boolean fRw;
		Boolean fUser;
		Boolean fExecDisable;
		UIntPtr fVirtAddr;
		Boolean fCache;
		Boolean fShareable;
		Boolean fWt;
		Boolean fPresent;
		Boolean fAccessed;

	private:
		friend class PageMgr;
		friend class Pmm;
	};

	struct PageMgr final
	{
	public:
		PageMgr()  = default;
		~PageMgr() = default;

		PageMgr& operator=(const PageMgr&) = default;
		PageMgr(const PageMgr&)			   = default;

	public:
		PTEWrapper Request(Boolean Rw, Boolean User, Boolean ExecDisable, SizeT Sz);
		bool	   Free(Ref<PTEWrapper>& wrapper);

	private:
		void FlushTLB();

	private:
		friend PTEWrapper;
		friend class Pmm;
	};
} // namespace Kernel
