// a way to create and find our pages.
// I'm thinking about a separate way of getting a paged area.

/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/PageAllocator.hxx>
#include <NewKit/Ref.hxx>

namespace Kernel
{
	class PageManager;

	class PTEWrapper final
	{
	public:
		explicit PTEWrapper(Boolean Rw = false, Boolean User = false, Boolean ExecDisable = false, UIntPtr Address = 0);

		~PTEWrapper();

		PTEWrapper& operator=(const PTEWrapper&) = default;
		PTEWrapper(const PTEWrapper&)			 = default;

	public:
		const UIntPtr VirtualAddress();

		void		NoExecute(const bool enable = false);
		const bool& NoExecute();

		operator bool() { return fVirtAddr; }

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
		friend class PageManager;
		friend class Pmm;
	};

	struct PageManager final
	{
	public:
		PageManager()  = default;
		~PageManager() = default;

		PageManager& operator=(const PageManager&) = default;
		PageManager(const PageManager&)			   = default;

	public:
		PTEWrapper Request(Boolean Rw, Boolean User, Boolean ExecDisable, SizeT Sz);
		bool	   Free(Ref<PTEWrapper*>& wrapper);

	private:
		void FlushTLB();

	private:
		friend PTEWrapper;
		friend class Pmm;
	};
} // namespace Kernel
