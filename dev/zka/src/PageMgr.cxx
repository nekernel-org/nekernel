/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/DebugOutput.hxx>
#include <NewKit/PageMgr.hxx>

#ifdef __ZKA_AMD64__
#include <HALKit/AMD64/Paging.hxx>
#elif defined(__ZKA_ARM64__)
#include <HALKit/ARM64/Paging.hxx>
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
		hal_flush_tlb();
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
		VoidPtr ptr = Kernel::HAL::mm_alloc_bitmap(Rw, User, Sz);

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

	bool PTEWrapper::Shareable()
	{
		return fShareable;
	}

	bool PTEWrapper::Present()
	{
		return fPresent;
	}

	bool PTEWrapper::Access()
	{
		return fAccessed;
	}

	void PTEWrapper::NoExecute(const bool enable)
	{
		this->fExecDisable = enable;
	}

	const bool& PTEWrapper::NoExecute()
	{
		return this->fExecDisable;
	}
} // namespace Kernel
