/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/LPC.h>
#include <NewKit/KernelPanic.h>

namespace Kernel
{
	STATIC Bool kRaiseOnBugCheck = false;

	/// @brief Does a system wide bug check.
	/// @param void no params.
	/// @return if error-free: false, otherwise true.
	Boolean err_bug_check(void) noexcept
	{
		if (kRaiseOnBugCheck)
		{
			ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR);
		}

		return No;
	}

	/// @brief Tells if we should raise a bug check not.
	/// @param void
	/// @return void
	Void err_bug_check_raise(Void) noexcept
	{
		kRaiseOnBugCheck = true;
	}
} // namespace Kernel
