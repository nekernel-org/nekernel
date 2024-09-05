/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <ArchKit/ArchKit.hxx>
#include <KernelKit/DebugOutput.hxx>
#include <NewKit/PageAllocator.hxx>

/// @brief Internal namespace, used internally by Kernel.
namespace Kernel::Detail
{
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
