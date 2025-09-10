/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/KPC.h>
#include <KernelKit/UserProcessScheduler.h>
#include <NeKit/CxxAbi.h>

atexit_func_entry_t __atexit_funcs[kAtExitMacDestructors];

uarch_t __atexit_func_count;

/// @brief dynamic shared object Handle.
Kernel::UIntPtr __dso_handle;

EXTERN_C Kernel::Void __cxa_pure_virtual(void* self) {
  (Kernel::Void)(Kernel::kout << "object: "
                              << Kernel::number(reinterpret_cast<Kernel::UIntPtr>(self)));
  (Kernel::Void)(Kernel::kout << ", has unimplemented virtual functions.\r");
}

EXTERN_C void ___chkstk_ms(PtrDiff frame_size) {
  char* sp;
  asm volatile("mov %%rsp, %0" : "=r"(sp));

  for (PtrDiff offset = kPageSize; offset < frame_size; offset += kPageSize) {
    sp[-offset] = 0;
  }
}

EXTERN_C int atexit(void (*f)()) {
  if (__atexit_func_count >= kAtExitMacDestructors) return 1;

  __atexit_funcs[__atexit_func_count].destructor_func = f;

  __atexit_func_count++;

  return 0;
}

EXTERN_C void __cxa_finalize(void* f) {
  uarch_t i = __atexit_func_count;
  if (!f) {
    while (i--) {
      if (__atexit_funcs[i].destructor_func) {
        (*__atexit_funcs[i].destructor_func)();
      };
    }

    return;
  }

  while (i--) {
    if (__atexit_funcs[i].destructor_func) {
      (*__atexit_funcs[i].destructor_func)();
      __atexit_funcs[i].destructor_func = 0;
    };
  }
}

namespace cxxabiv1 {
EXTERN_C int __cxa_guard_acquire(__guard g) {
  if ((*g & 1) || (*g && 2)) return 1;
  *g |= 2;
  return 0;
}

EXTERN_C void __cxa_guard_release(__guard g) {
  *g |= 1;
  *g &= 2;
}

EXTERN_C void __cxa_guard_abort(__guard g) {
  *g &= ~2;
}
}  // namespace cxxabiv1
