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

Boolean hcore_tls_check(VoidPtr ptr) {
  if (!ptr) return false;

  const char* _ptr = (const char*)ptr;

  kcout << "HCoreKrnl\\TLS: Checking for cookie...\n";

  return _ptr[0] == kCookieMag0 && _ptr[1] == kCookieMag1 &&
         _ptr[2] == kCookieMag2;
}

/**
 * System call implementation in HCore
 * @param ptr
 * @return
 */
EXTERN_C Void hcore_tls_check_syscall_impl(HCore::HAL::StackFramePtr stackPtr) noexcept {
  ThreadInformationBlock* tib = (ThreadInformationBlock*)stackPtr->Gs;

  if (!hcore_tls_check(tib->Cookie)) {
    kcout << "HCoreKrnl\\TLS: Verification failed, Crashing...\n";
    ProcessManager::Shared().Leak().GetCurrent().Leak().Crash();
  }

  kcout << "HCoreKrnl\\TLS: Verification succeeded! Keeping on...\n";
}
