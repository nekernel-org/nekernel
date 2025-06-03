
/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef _REF_H_
#define _REF_H_

#include <CoreFoundation.fwrk/headers/Object.h>
#include <libSystem/SystemKit/System.h>

namespace CF {
template <typename T>
class CFRef;

template <typename T>
class CFRef final CF_OBJECT {
 public:
  CFRef() = default;

  ~CFRef() {
    if (MmGetHeapFlags(fClass) != -1) delete fClass;
  }

 public:
  CFRef(T* cls) : fClass(cls) {}

  CFRef(T cls) : fClass(&cls) {}

  CFRef& operator=(T ref) {
    if (!fClass) return *this;

    fClass = &ref;
    return *this;
  }

 public:
  T operator->() const {
    MUST_PASS(*fClass);
    return *fClass;
  }

  T& Leak() noexcept { return *fClass; }

  T& TryLeak() const noexcept {
    MUST_PASS(*fClass);
    return *fClass;
  }

  T operator*() { return *fClass; }

  operator bool() noexcept { return fClass; }

 private:
  T* fClass{nullptr};
};

template <typename T>
class CFNonNullRef final {
 public:
  CFNonNullRef()        = delete;
  CFNonNullRef(nullPtr) = delete;

  CFNonNullRef(T* ref) : fRef(ref) { MUST_PASS(ref); }

  CFRef<T>& operator->() {
    MUST_PASS(fRef);
    return fRef;
  }

  CFNonNullRef& operator=(const CFNonNullRef<T>& ref) = delete;
  CFNonNullRef(const CFNonNullRef<T>& ref)            = default;

 private:
  CFRef<T> fRef{nullptr};
};
}  // namespace CF

#endif  // ifndef _NEWKIT_REF_H_
