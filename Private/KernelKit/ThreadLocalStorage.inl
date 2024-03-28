/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

//! @brief Allocates a pointer from the process's tls.

#ifndef __PROCESS_MANAGER__
#include <KernelKit/ProcessScheduler.hpp>
#endif

template <typename T>
inline T* tls_new_ptr(void) {
  using namespace NewOS;

  MUST_PASS(ProcessScheduler::Shared().Leak().GetCurrent());

  auto ref_process = ProcessScheduler::Shared().Leak().GetCurrent();

  T* pointer = (T*)ref_process.Leak().New(sizeof(T));
  return pointer;
}

//! @brief TLS delete implementation.
template <typename T>
inline bool tls_delete_ptr(T* ptr) {
  if (!ptr) return false;

  using namespace NewOS;

  MUST_PASS(ProcessScheduler::Shared().Leak().GetCurrent());

  ptr->~T();

  auto ref_process = ProcessScheduler::Shared().Leak().GetCurrent();
  return ref_process.Leak().Delete(ptr, sizeof(T));
}

template <typename T, typename... Args>
T* tls_new_class(Args&&... args) {
  T* ptr = tls_new_ptr<T>();

  if (ptr) {
    *ptr = T(NewOS::forward(args)...);
    return ptr;
  }

  return nullptr;
}
