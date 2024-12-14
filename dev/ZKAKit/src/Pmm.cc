/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <NewKit/Pmm.h>

#if defined(__ZKA_ARM64__)
#include <HALKit/ARM64/Processor.h>
#endif // defined(__ZKA_ARM64__)

#if defined(__ZKA_AMD64__)
#include <HALKit/AMD64/Processor.h>
#endif // defined(__ZKA_AMD64__)

namespace Kernel
{
	/***********************************************************************************/
	/// @brief Pmm constructor.
	/***********************************************************************************/
	Pmm::Pmm()
		: fPageMgr()
	{
		kcout << "[PMM] Allocate PageMemoryMgr";
	}

	Pmm::~Pmm() = default;

	/***********************************************************************************/
	/// @param If this returns Null pointer, enter emergency mode.
	/// @param user is this a user page?
	/// @param readWrite is it r/w?
	/***********************************************************************************/
	Ref<PTEWrapper> Pmm::RequestPage(Boolean user, Boolean readWrite)
	{
		PTEWrapper pt = fPageMgr.Leak().Request(user, readWrite, false, kPageSize);

		if (pt.fPresent)
		{
			kcout << "[PMM]: Allocation failed.\r";
			return {};
		}

		return Ref<PTEWrapper>(pt);
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
