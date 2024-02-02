/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

//! @brief Allocates a pointer from the process's tls.

#ifndef __PROCESS_MANAGER__
#include <KernelKit/ProcessManager.hpp>
#endif

template <typename T>
inline T* hcore_tls_new_ptr(void) {
  using namespace HCore;

  auto ref_process = ProcessManager::Shared().Leak().GetCurrent();

  T* pointer = (T*)ref_process.Leak().New(sizeof(T));
  return pointer;
}

//! @brief TLS delete implementation.
template <typename T>
inline bool hcore_tls_delete_ptr(T* ptr) {
  if (!ptr) return false;

  using namespace HCore;

  auto ref_process = ProcessManager::Shared().Leak().GetCurrent();
  ptr->~T();

  return ref_process.Leak().Delete(ptr, sizeof(T));
}

template <typename T, typename... Args>
T* hcore_tls_new_class(Args&&... args) {
  T* ptr = hcore_tls_new_ptr<T>();

  if (ptr) {
    *ptr = T(HCore::forward(args)...);
    return ptr;
  }

  return nullptr;
}
