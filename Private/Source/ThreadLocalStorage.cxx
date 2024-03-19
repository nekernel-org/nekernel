/*
 * ========================================================
 *
 * HCore
 * Copyright Mahrouss Logic, all rights reserved.
 *
 *  ========================================================
 */

#include <KernelKit/ProcessScheduler.hpp>
#include <KernelKit/ThreadLocalStorage.hxx>

/// bugs 0

/***********************************************************************************/
/// @file ThreadLocalStorage.cxx
/// @brief TLS implementation in kernel.
/***********************************************************************************/

using namespace HCore;

/**
 * Check for cookie inside TIB.
 * @param ptr
 * @return if the cookie is enabled.
 */

Boolean tls_check_tib(ThreadInformationBlock* tib) {
  if (!tib) return false;

  HCore::Encoder encoder;
  const char* tibAsBytes = encoder.AsBytes(tib);

  kcout << "HCoreKrnl\\TLS: Checking for a valid cookie...\n";

  return tibAsBytes[0] == kCookieMag0 && tibAsBytes[1] == kCookieMag1 &&
         tibAsBytes[2] == kCookieMag2;
}

/**
 * System call implementation of the TLS check.
 * @param ptr
 * @return
 */
EXTERN_C Void tls_check_syscall_impl(HCore::HAL::StackFramePtr stackPtr) noexcept {
  ThreadInformationBlock* tib = (ThreadInformationBlock*)stackPtr->Gs;

  if (!tls_check_tib(tib)) {
    kcout << "HCoreKrnl\\TLS: Verification failed, Crashing...\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
  }

  kcout << "HCoreKrnl\\TLS: Verification succeeded! Keeping on...\n";
}
