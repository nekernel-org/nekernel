
/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/Defines.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/Ref.h>

namespace Kernel {
template <typename T>
class OwnPtr;

template <typename T>
class NonNullRefPtr;

template <typename T>
class OwnPtr final {
 public:
  OwnPtr() {}
  ~OwnPtr() { this->Delete(); }

  OwnPtr& operator=(const OwnPtr&) = default;
  OwnPtr(const OwnPtr&)            = default;

 public:
  template <typename... Args>
  bool New(Args&&... arg) {
    if (fCls) {
      return false;
    }

    fCls = new T(arg...);
    return fCls;
  }

  void Delete() {
    if (fCls) delete fCls;

    fCls = nullptr;
  }

  T* operator->() const { return fCls; }

  T* Raw() { return fCls; }

  Ref<T> AsRef() { return Ref<T>(fCls); }

       operator bool() { return fCls; }
  bool operator!() { return !fCls; }

 private:
  T* fCls;
};

template <typename T, typename... Args>
inline OwnPtr<T> mm_make_own_ptr(Args... args) {
  OwnPtr<T> ret;
  ret.template New<Args...>(forward(args)...);
  MUST_PASS(ret);

  return ret;
}
}  // namespace Kernel
