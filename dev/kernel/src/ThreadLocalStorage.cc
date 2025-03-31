/*
 * ========================================================
 *
 * neoskrnl
 * Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *  ========================================================
 */

#include <NewKit/KString.h>
#include <CFKit/Property.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/ThreadLocalStorage.h>

/***********************************************************************************/
/// @bugs: 0
/// @file ThreadLocalStorage.cc
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
		!tib_ptr->Record)
		return false;

	ICodec		encoder;
	const Char* tib_as_bytes = encoder.AsBytes(tib_ptr);

	kout << "TLS: Validating the TIB...\r";

	return tib_as_bytes[kCookieMag0Idx] == kCookieMag0 && tib_as_bytes[kCookieMag1Idx] == kCookieMag1 &&
		   tib_as_bytes[kCookieMag2Idx] == kCookieMag2;
}

/**
 * @brief System call implementation of the TLS check.
 * @param tib_ptr The TIB record.
 * @return if the TIB record is valid or not.
 */
EXTERN_C Bool tls_check_syscall_impl(Kernel::VoidPtr tib_ptr) noexcept
{
	if (!tib_ptr)
	{
		kout << "TLS: Failed because of an invalid TIB...\r";
		return No;
	}

	THREAD_INFORMATION_BLOCK* tib = reinterpret_cast<THREAD_INFORMATION_BLOCK*>(tib_ptr);

	if (!tls_check_tib(tib))
	{
		kout << "TLS: Failed because of an invalid TIB...\r";
		return No;
	}

	kout << "TLS Pass.\r";
	return Yes;
}
