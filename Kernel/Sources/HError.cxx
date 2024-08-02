/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <KernelKit/LPC.hxx>
#include <NewKit/KernelCheck.hxx>

namespace Kernel
{
	STATIC Bool cRaise = false;

	/// @brief Does a system wide bug check.
	/// @param void no params.
	/// @return if error-free: true, otherwise false.
	Boolean err_bug_check(void) noexcept
	{
		if (cRaise)
		{
			ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
		}

		return false;
	}

	Void err_bug_check_raise(void) noexcept
	{
		if (!cRaise)
			cRaise = true;
	}
} // namespace Kernel
