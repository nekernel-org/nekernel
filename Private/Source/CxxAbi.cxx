/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <NewKit/CxxAbi.hpp>
#include <NewKit/RuntimeCheck.hpp>

#ifdef __GNUC__

void *__dso_handle;

atexit_func_entry_t __atexit_funcs[DSO_MAX_OBJECTS];
uarch_t __atexit_func_count;

extern "C" void __cxa_pure_virtual() { HCore::kcout << "[C++] Placeholder\n"; }

extern "C" void __stack_chk_fail() {
  HCore::kcout << "[HCoreKrnl] Buffer overflow detected\n";
  HCore::ke_stop(RUNTIME_CHECK_POINTER);
}

extern "C" int __cxa_atexit(void (*f)(void *), void *arg, void *dso) {
  if (__atexit_func_count >= DSO_MAX_OBJECTS) return -1;

  __atexit_funcs[__atexit_func_count].destructor_func = f;
  __atexit_funcs[__atexit_func_count].obj_ptr = arg;
  __atexit_funcs[__atexit_func_count].dso_handle = dso;

  __atexit_func_count++;

  return 0;
}

extern "C" void __cxa_finalize(void *f) {
  uarch_t i = __atexit_func_count;
  if (!f) {
    while (i--) {
      if (__atexit_funcs[i].destructor_func) {
        (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
      };
    }

    return;
  }

  while (i--) {
    if (__atexit_funcs[i].destructor_func) {
      (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
      __atexit_funcs[i].destructor_func = 0;
    };
  }
}

namespace cxxabiv1 {
extern "C" int __cxa_guard_acquire(__guard *g) {
  (void)g;
  return 0;
}

extern "C" int __cxa_guard_release(__guard *g) {
  *(char *)g = 1;
  return 0;
}

extern "C" void __cxa_guard_abort(__guard *g) { (void)g; }
}  // namespace cxxabiv1

#else

namespace cxxkit {
extern "C" void __unwind(void (**finis)(void), int cnt) {
  for (int i = 0; i < cnt; ++i) (finis[i])();
}

extern "C" void __init_local(void (**init)(void), int cnt) {
  for (int i = 0; i < cnt; ++i) (init[i])();
}

extern "C" void __fini_local(void (**finis)(void), int cnt) {
  for (int i = 0; i < cnt; ++i) (finis[i])();
}
}  // namespace cxxkit

#endif
