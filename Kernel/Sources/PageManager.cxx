/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/DebugOutput.hxx>
#include <NewKit/PageManager.hxx>

#ifdef __NEWOS_AMD64__
#include <HALKit/AMD64/HalPageAlloc.hxx>
#elif defined(__NEWOS_ARM64__)
#include <HALKit/ARM64/HalPageAlloc.hxx>
#endif // ifdef __NEWOS_AMD64__ || defined(__NEWOS_ARM64__)

//! null deref will throw (Page Zero detected, aborting app!)
#define kProtectedRegionEnd (512)

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
	Void PageManager::FlushTLB(UIntPtr VirtAddr)
	{
		if (VirtAddr == kBadAddress)
			return;

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
	PTEWrapper PageManager::Request(Boolean Rw, Boolean User, Boolean ExecDisable, SizeT Sz)
	{
		// Store PTE wrapper right after PTE.
		VoidPtr ptr = Kernel::HAL::hal_alloc_page(Rw, User, Sz);
		
		if (ptr == kBadAddress)
		{
			kcout << "[create_page_wrapper] kBadAddress returned\n";
			ke_stop(RUNTIME_CHECK_POINTER);
		}

		return PTEWrapper{Rw, User, ExecDisable, reinterpret_cast<UIntPtr>(ptr)};
	}

	/// @brief Disable PTE.
	/// @param wrapper the wrapper.
	/// @return
	bool PageManager::Free(Ref<PTEWrapper*>& wrapper)
	{
		if (wrapper)
		{
			if (!Detail::page_disable(wrapper->VirtualAddress()))
				return false;
			return true;
		}

		return false;
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
