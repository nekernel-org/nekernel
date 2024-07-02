/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Pmm.hpp>

#if defined(__NEWOS_ARM64__)
#include <HALKit/ARM64/Processor.hxx>
#endif

#if defined(__NEWOS_AMD64__)
#include <HALKit/AMD64/Processor.hpp>
#endif

namespace Kernel
{
	/// @brief Pmm constructor.
	Pmm::Pmm()
		: fPageManager()
	{
		kcout << "[PMM] Allocate PageMemoryManager";
	}

	Pmm::~Pmm() = default;

	/* If this returns Null pointer, enter emergency mode */
	/// @param user is this a user page?
	/// @param readWrite is it r/w?
	Ref<PTEWrapper> Pmm::RequestPage(Boolean user, Boolean readWrite)
	{
		PTEWrapper pt = fPageManager.Leak().Request(user, readWrite, false, kPTESize);

		if (pt.fPresent)
		{
			kcout << "[PMM]: Allocation was successful.\r";
			return Ref<PTEWrapper>(pt);
		}

		kcout << "[PMM]: Allocation failed.\r";

		return {};
	}

	Boolean Pmm::FreePage(Ref<PTEWrapper> PageRef)
	{
		if (!PageRef)
			return false;

		PageRef.Leak().fPresent = false;

		return true;
	}

	Boolean Pmm::TogglePresent(Ref<PTEWrapper> PageRef, Boolean Enable)
	{
		if (!PageRef)
			return false;

		PageRef.Leak().fPresent = Enable;

		return true;
	}

	Boolean Pmm::ToggleUser(Ref<PTEWrapper> PageRef, Boolean Enable)
	{
		if (!PageRef)
			return false;

		PageRef.Leak().fRw = Enable;

		return true;
	}

	Boolean Pmm::ToggleRw(Ref<PTEWrapper> PageRef, Boolean Enable)
	{
		if (!PageRef)
			return false;

		PageRef.Leak().fRw = Enable;

		return true;
	}

	Boolean Pmm::ToggleShare(Ref<PTEWrapper> PageRef, Boolean Enable)
	{
		if (!PageRef)
			return false;

		PageRef.Leak().fShareable = Enable;

		return true;
	}
} // namespace Kernel
