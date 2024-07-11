/*
 * ========================================================
 *
 * Kernel
 * Copyright ZKA Technologies, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/ProcessScheduler.hxx>
#include <KernelKit/ThreadLocalStorage.hxx>

///! BUGS: 0

/***********************************************************************************/
/// @file ThreadLocalStorage.cxx
/// @brief TLS implementation in kernel.
/***********************************************************************************/

using namespace Kernel;

/**
 * @brief Check for cookie inside TIB.
 * @param tib the TIB to check.
 * @return if the cookie is enabled.
 */

Boolean tls_check_tib(ThreadInformationBlock* tib)
{
	if (!tib)
		return false;

	Encoder		encoder;
	const char* tibAsBytes = encoder.AsBytes(tib);

	kcout << "newoskrnl: Checking for a valid cookie...\r";

	return tibAsBytes[0] == kCookieMag0 && tibAsBytes[1] == kCookieMag1 &&
		   tibAsBytes[2] == kCookieMag2;
}

/**
 * @brief System call implementation of the TLS check.
 * @param stackPtr The call frame.
 * @return
 */
EXTERN_C Void tls_check_syscall_impl(Kernel::VoidPtr TIB) noexcept
{
	if (!TIB)
		return;

	ThreadInformationBlock* tib = (ThreadInformationBlock*)TIB;

	if (!tls_check_tib(tib))
	{
		kcout << "newoskrnl: crashing because of an invalid TIB...\r";
		ProcessScheduler::The().Leak().TheCurrent().Leak().Crash();
	}

	kcout << "newoskrnl: Verification succeeded! Keeping on...\r";
}
