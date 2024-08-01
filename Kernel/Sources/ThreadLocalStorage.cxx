/*
 * ========================================================
 *
 * Kernel
 * Copyright ZKA Technologies, all rights reserved.
 *
 *  ========================================================
 */

#include <NewKit/String.hpp>
#include <CFKit/Property.hpp>
#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>

///! BUGS: 0

/***********************************************************************************/
/// @file ThreadLocalStorage.cxx
/// @brief TLS inside the kernel.
/***********************************************************************************/

using namespace Kernel;

Kernel::Property cTLSEnforceCheck;

/**
 * @brief Checks for cookie inside the TIB.
 * @param tib the TIB to check.
 * @return if the cookie is enabled.
 */

Boolean tls_check_tib(THREAD_INFORMATION_BLOCK* the_tib)
{
	if (!the_tib)
		return false;

	Encoder		encoder;
	const char* tibAsBytes = encoder.AsBytes(the_tib);

	kcout << "newoskrnl: checking for a valid cookie inside the TIB...\r";

	return tibAsBytes[0] == kCookieMag0 && tibAsBytes[1] == kCookieMag1 &&
		   tibAsBytes[2] == kCookieMag2;
}

/**
 * @brief System call implementation of the TLS check.
 * @param stackPtr The call frame.
 * @return
 */
EXTERN_C Bool tls_check_syscall_impl(Kernel::VoidPtr tib_ptr) noexcept
{
	if (!tib_ptr)
	{
		if (cTLSEnforceCheck.GetValue() == No)
		{
			return true;
		}
		else
		{
			kcout << "newoskrnl: failing because of an invalid TIB...\r";
			return false;
		}
	}

	THREAD_INFORMATION_BLOCK* tib_struct = (THREAD_INFORMATION_BLOCK*)tib_ptr;

	if (!tls_check_tib(tib_struct))
	{
		kcout << "newoskrnl: crashing because of an invalid TIB...\r";
		return false;
	}

	kcout << "newoskrnl: Verification succeeded! staying alive...\r";
	return true;
}
