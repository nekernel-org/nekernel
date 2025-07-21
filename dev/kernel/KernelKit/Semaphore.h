/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

/// @author Amlal El Mahrouss
/// @file Semaphore.h
/// @brief Semaphore structure and functions for synchronization in the kernel.

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/Timer.h>
#include <NeKit/Defines.h>

#define kSemaphoreOwnerIndex (0)
#define kSemaphoreCountIndex (1)

#define kSemaphoreCount (2)

namespace Kernel {
/// @brief Semaphore structure used for synchronization.
typedef UInt64 Semaphore[kSemaphoreCount];

/// @brief Checks if the semaphore is valid.
inline BOOL rtl_sem_is_valid(const Semaphore& sem) {
  return sem[kSemaphoreOwnerIndex] != 0 || sem[kSemaphoreCountIndex] > 0;
}

/// @brief Releases the semaphore, resetting its owner and count.
/// @param sem
/// @return
inline BOOL rtl_sem_release(Semaphore& sem) {
  sem[kSemaphoreOwnerIndex] = 0;
  sem[kSemaphoreCountIndex] = 0;

  return TRUE;
}

/// @brief Initializes the semaphore with an owner and a count of zero.
/// @param sem
/// @param owner
/// @return
inline BOOL rtl_sem_init(Semaphore& sem, Int64 owner) {
  if (!owner || sem[kSemaphoreOwnerIndex] > 0) {
    return FALSE;
  }

  sem[kSemaphoreOwnerIndex] = owner;
  sem[kSemaphoreCountIndex] = 0;

  return TRUE;
}

/// @brief Waits for the semaphore to be available, blocking until it is.
/// @param sem
/// @param timeout
/// @return
inline BOOL rtl_sem_wait(Semaphore& sem, Int64 timeout) {
  if (!rtl_sem_is_valid(sem)) {
    return FALSE;
  }

  if (timeout <= 0) {
    err_global_get() = kErrorNetworkTimeout;

    return FALSE;
  }

  if (sem[kSemaphoreCountIndex] > 0) {
    err_global_get() = kErrorSuccess;
    sem[kSemaphoreCountIndex]--;

    return TRUE;
  }

  HardwareTimer timer(timeout);
  timer.Wait();
  
  if (sem[kSemaphoreCountIndex] > 0) {
    err_global_get() = kErrorSuccess;
    sem[kSemaphoreCountIndex]--;

    return TRUE;
  }

  err_global_get() = kErrorNetworkTimeout;

  return FALSE;  // Failed to acquire semaphore
}  // namespace Kernel
}  // namespace Kernel