
/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef _NEWKIT_REF_H_
#define _NEWKIT_REF_H_

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/HeapMgr.h>
#include <NeKit/Defines.h>
#include <NeKit/KernelPanic.h>

namespace Kernel {
template <typename T>
class Ref final {
 public:
  explicit Ref() = default;
  ~Ref()         = default;

 public:
  Ref(T* cls) : fClass(*cls) {}
  Ref(T cls) : fClass(cls) {}

  Ref& operator=(T ref) {
    fClass = ref;
    return *this;
  }

  NE_COPY_DEFAULT(Ref);

 public:
  T operator->() const { return fClass; }

  T& Leak() noexcept { return fClass; }

  T& TryLeak() const noexcept { return fClass; }

  T operator*() { return fClass; }

  operator bool() noexcept { return true; }

 private:
  T fClass;
};

template <typename T>
class NonNullRef final {
 public:
  NonNullRef()        = delete;
  NonNullRef(nullPtr) = delete;

  NonNullRef(T* ref) : fRef(ref) { MUST_PASS(ref); }

  Ref<T>& operator->() {
    MUST_PASS(fRef);
    return fRef;
  }

  NonNullRef& operator=(const NonNullRef<T>& ref) = delete;
  NonNullRef(const NonNullRef<T>& ref)            = default;

 private:
  Ref<T> fRef{};
};
}  // namespace Kernel

#endif  // ifndef _NEWKIT_REF_H_
