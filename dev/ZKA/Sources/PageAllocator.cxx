/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <NewKit/PageAllocator.hxx>

/// @brief Internal namespace, used internally by kernel.
namespace Kernel::Detail
{
	VoidPtr create_page_wrapper(Boolean rw, Boolean user, SizeT pageSz)
	{
		auto addr = HAL::hal_alloc_page(rw, user, pageSz);

		if (addr == kBadAddress)
		{
			kcout << "[create_page_wrapper] kBadAddress returned\n";
			ke_stop(RUNTIME_CHECK_POINTER);
		}

		return addr;
	}

	void exec_disable(UIntPtr VirtualAddr)
	{
#ifdef __ZKA_SUPPORT_NX__
		PTE* VirtualAddrTable = reinterpret_cast<PTE*>(VirtualAddr);

		MUST_PASS(!VirtualAddrTable->ExecDisable == false);
		VirtualAddrTable->ExecDisable = true;

		hal_flush_tlb();
#endif // ifdef __ZKA_SUPPORT_NX__
	}

	bool page_disable(UIntPtr VirtualAddr)
	{
		if (VirtualAddr)
		{
			auto VirtualAddrTable = (PTE*)(VirtualAddr);

			MUST_PASS(!VirtualAddrTable->Present == true);
			VirtualAddrTable->Present = false;

			hal_flush_tlb();

			return true;
		}

		return false;
	}
} // namespace Kernel::Detail
