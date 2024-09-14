/*
 * ========================================================
 *
 * newoskrnl
 * Copyright ZKA Technologies., all rights reserved.
 *
 *  ========================================================
 */

#include <NewKit/String.hxx>
#include <CFKit/Property.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>

///! BUGS: 0

/***********************************************************************************/
/// @file ThreadLocalStorage.cxx
/// @brief TLS inside the Kernel.
/***********************************************************************************/

using namespace Kernel;

/**
 * @brief Checks for cookie inside the TIB.
 * @param tib the TIB to check.
 * @return if the cookie is enabled.
 */

Boolean tls_check_tib(THREAD_INFORMATION_BLOCK* the_tib)
{
	if (!the_tib ||
		!the_tib->f_ThreadRecord)
		return false;

	IEncoderObject encoder;
	const char*	   tibAsBytes = encoder.AsBytes(the_tib);

	kcout << "checking for a valid cookie inside the TIB...\r";

	return tibAsBytes[0] == kCookieMag0 && tibAsBytes[1] == kCookieMag1 &&
		   tibAsBytes[2] == kCookieMag2;
}

/**
 * @brief System call implementation of the TLS check.
 * @param tib_ptr The TIB record.
 * @return
 */
EXTERN_C Bool tls_check_syscall_impl(Kernel::VoidPtr tib_ptr) noexcept
{
	if (!tib_ptr)
	{
		kcout << "failing because of an invalid TIB...\r";
		return false;
	}

	THREAD_INFORMATION_BLOCK* tib_struct = (THREAD_INFORMATION_BLOCK*)tib_ptr;

	if (!tls_check_tib(tib_struct))
	{
		kcout << "crashing because of an invalid TIB...\r";
		return false;
	}

	kcout << "Verification succeeded! staying alive...\r";
	return true;
}
