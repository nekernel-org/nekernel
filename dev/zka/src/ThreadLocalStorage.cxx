/*
 * ========================================================
 *
 * minkrnl
 * Copyright ZKA Technologies., all rights reserved.
 *
 *  ========================================================
 */

#include <NewKit/String.hxx>
#include <CFKit/Property.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>

/***********************************************************************************/
/// @bugs: 0
/// @file ThreadLocalStorage.cxx
/// @brief Process Thread Local Storage.
/***********************************************************************************/

using namespace Kernel;

/**
 * @brief Checks for cookie inside the TIB.
 * @param tib_ptr the TIB to check.
 * @return if the cookie is enabled, true; false otherwise
 */

Boolean tls_check_tib(THREAD_INFORMATION_BLOCK* tib_ptr)
{
	if (!tib_ptr ||
		!tib_ptr->f_ThreadRecord)
		return false;

	IEncoderObject encoder;
	const char*	   tib_as_bytes = encoder.AsBytes(tib_ptr);

	kcout << "Checking for a valid cookie inside the TIB...\r";

	return tib_as_bytes[0] == kCookieMag0 && tib_as_bytes[1] == kCookieMag1 &&
		   tib_as_bytes[2] == kCookieMag2;
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
		kcout << "Failing because of an invalid TIB...\r";
		return false;
	}

	THREAD_INFORMATION_BLOCK* tib = (THREAD_INFORMATION_BLOCK*)tib_ptr;

	if (!tls_check_tib(tib))
	{
		kcout << "Crashing because of an invalid TIB...\r";
		return false;
	}

	kcout << "Verification succeeded! staying alive...\r";
	return true;
}
