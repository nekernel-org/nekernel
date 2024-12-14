/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <NewKit/PageMgr.h>

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/Paging.h>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/Paging.h>
#endif // ifdef __ZKA_AMD64__ || defined(__ZKA_ARM64__)

namespace Kernel
{
	PTEWrapper::PTEWrapper(Boolean Rw, Boolean User, Boolean ExecDisable, UIntPtr VirtAddr)
		: fRw(Rw),
		  fUser(User),
		  fExecDisable(ExecDisable),
		  fVirtAddr(VirtAddr),
		  fCache(false),
		  fShareable(false),
		  fWt(false),
		  fPresent(true),
		  fAccessed(false)
	{
	}

	PTEWrapper::~PTEWrapper() = default;

	/// @brief Flush virtual address.
	/// @param VirtAddr
	Void PageMgr::FlushTLB()
	{
#ifndef __ZKA_MINIMAL_OS__
		hal_flush_tlb();
#endif // !__ZKA_MINIMAL_OS__
	}

	/// @brief Reclaim freed page.
	/// @return
	Bool PTEWrapper::Reclaim()
	{
		if (!this->fPresent)
		{
			this->fPresent = true;
			return true;
		}

		return false;
	}

	/// @brief Request a PTE.
	/// @param Rw r/w?
	/// @param User user mode?
	/// @param ExecDisable disable execution on page?
	/// @return
	PTEWrapper PageMgr::Request(Boolean Rw, Boolean User, Boolean ExecDisable, SizeT Sz)
	{
		// Store PTE wrapper right after PTE.
		VoidPtr ptr = Kernel::HAL::mm_alloc_bitmap(Rw, User, Sz, false);

		return PTEWrapper{Rw, User, ExecDisable, reinterpret_cast<UIntPtr>(ptr)};
	}

	/// @brief Disable BitMap.
	/// @param wrapper the wrapper.
	/// @return If the page bitmap was cleared or not.
	Bool PageMgr::Free(Ref<PTEWrapper>& wrapper)
	{
		if (!Kernel::HAL::mm_free_bitmap((VoidPtr)wrapper.Leak().VirtualAddress()))
			return false;

		return true;
	}

	/// @brief Virtual PTE address.
	/// @return The virtual address of the page.
	const UIntPtr PTEWrapper::VirtualAddress()
	{
		return (fVirtAddr);
	}

	Bool PTEWrapper::Shareable()
	{
		return fShareable;
	}

	Bool PTEWrapper::Present()
	{
		return fPresent;
	}

	Bool PTEWrapper::Access()
	{
		return fAccessed;
	}

	Void PTEWrapper::NoExecute(const bool enable)
	{
		fExecDisable = enable;
	}

	Bool PTEWrapper::NoExecute()
	{
		return fExecDisable;
	}
} // namespace Kernel
