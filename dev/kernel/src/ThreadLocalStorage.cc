/*
 * ========================================================
 *
 * NeKernel
 * Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *  ========================================================
 */

#include <CFKit/Property.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/ThreadLocalStorage.h>
#include <NeKit/KString.h>

/***********************************************************************************/
/// @bugs: 0
/// @file ThreadLocalStorage.cc
/// @brief NeKernel Thread Local Storage.
///! @author Amlal El Mahrouss (amlal@nekernel.org)
/***********************************************************************************/

using namespace Kernel;

/**
 * @brief Checks for cookie inside the TIB.
 * @param tib_ptr the TIB to check.
 * @return if the cookie is enabled, true; false otherwise
 */

Boolean tls_check_tib(THREAD_INFORMATION_BLOCK* tib_ptr) {
  if (!tib_ptr) return false;

  return tib_ptr->Cookie[kCookieMag0Idx] == kCookieMag0 &&
         tib_ptr->Cookie[kCookieMag1Idx] == kCookieMag1 &&
         tib_ptr->Cookie[kCookieMag2Idx] == kCookieMag2;
}

/**
 * @brief System call implementation of the TLS check.
 * @param tib_ptr The TIB record.
 * @return if the TIB record is valid or not.
 */
EXTERN_C Bool tls_check_syscall_impl(Kernel::VoidPtr tib_ptr) noexcept {
  if (!tib_ptr) {
    kout << "TLS: Failed because of an invalid TIB...\r";
    return No;
  }

  THREAD_INFORMATION_BLOCK* tib = reinterpret_cast<THREAD_INFORMATION_BLOCK*>(tib_ptr);

  return tls_check_tib(tib);
}
