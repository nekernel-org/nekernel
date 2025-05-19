/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/DebugOutput.h>
#include <NewKit/Defines.h>
#include <NewKit/ErrorOr.h>

namespace Kernel {
template <typename T, SizeT N>
class Array final {
 public:
  explicit Array() = default;
  ~Array()         = default;

  Array& operator=(const Array&) = default;
  Array(const Array&)            = default;

  T& operator[](SizeT at) { return fArray[at]; }

  Boolean Empty() { return this->Count() > 0; }

  SizeT Capacity() { return N; }

  SizeT Count() {
    const static SizeT kArrCnt = N;
    return kArrCnt;  // avoid constexpr error.
  }

  const T* CData() { return fArray; }

  operator bool() { return !Empty(); }

 private:
  T fArray[N];
};

template <typename ValueType>
auto make_list(ValueType val) {
  return Array<ValueType, ARRAY_SIZE(val)>{val};
}
}  // namespace Kernel
