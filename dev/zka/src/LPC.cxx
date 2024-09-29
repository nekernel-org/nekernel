/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <KernelKit/LPC.hxx>
#include <NewKit/Stop.hxx>

namespace Kernel
{
	STATIC Bool cRaise = false;

	/// @brief Does a system wide bug check.
	/// @param void no params.
	/// @return if error-free: false, otherwise true.
	Boolean err_bug_check(void) noexcept
	{
		if (cRaise)
		{
			ke_stop(RUNTIME_CHECK_BAD_BEHAVIOR);
		}

		return false;
	}

	/// @brief Tells if we should raise a bug check not.
	/// @param  void
	/// @return void
	Void err_bug_check_raise(Void) noexcept
	{
		cRaise = true;
	}
} // namespace Kernel
