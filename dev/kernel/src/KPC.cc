/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/KPC.h>
#include <NewKit/KernelPanic.h>
#include <KernelKit/MemoryMgr.h>

namespace NeOS
{
	STATIC Bool kRaiseOnBugCheck = false;

	/// @brief Does a system wide bug check.
	/// @param void no params are needed.
	/// @return if error-free: false, otherwise true.
	Boolean err_bug_check_raise(Void) noexcept
	{
		Char* ptr = new Char[512];

		if (ptr == nullptr)
			goto bug_check_fail;

		if (!mm_is_valid_heap(ptr))
			goto bug_check_fail;

		delete[] ptr;

		return Yes;

	bug_check_fail:
		if (ptr)
			delete[] ptr;

		ptr = nullptr;

		if (kRaiseOnBugCheck)
		{
			ke_panic(RUNTIME_CHECK_BAD_BEHAVIOR);
		}

		return No;
	}
} // namespace NeOS
