/*
 * ========================================================
 *
 * NewOS
 * Copyright SoftwareLabs, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>

///! BUGS: 0

/***********************************************************************************/
/// @file ThreadLocalStorage.cxx
/// @brief TLS implementation in kernel.
/***********************************************************************************/

using namespace NewOS;

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

	kcout << "New OS: Checking for a valid cookie...\r";

	return tibAsBytes[0] == kCookieMag0 && tibAsBytes[1] == kCookieMag1 &&
		   tibAsBytes[2] == kCookieMag2;
}

/**
 * @brief System call implementation of the TLS check.
 * @param stackPtr The call frame.
 * @return
 */
EXTERN_C Void tls_check_syscall_impl(NewOS::HAL::StackFramePtr stackPtr) noexcept
{
	ThreadInformationBlock* tib = (ThreadInformationBlock*)stackPtr->Gs;

	if (!tls_check_tib(tib))
	{
		kcout << "New OS: Verification failed, Crashing...\r";
		ProcessScheduler::Shared().Leak().GetCurrent().Leak().Crash();
	}

	kcout << "New OS: Verification succeeded! Keeping on...\r";
}
