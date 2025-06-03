/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

namespace CF {
template <typename T, SizeT N>
class CFArray final {
 public:
  explicit CFArray() = default;
  ~CFArray()         = default;

  CFArray& operator=(const CFArray&) = default;
  CFArray(const CFArray&)            = default;

  T& operator[](SizeT at) {
    MUST_PASS(at < this->Count());
    return fArray[at];
  }

  Bool Empty() { return this->Count() > 0; }

  SizeT Capacity() { return N; }

  SizeT Count() {
    auto cnt = 0UL;

    for (auto i = 0UL; i < N; ++i) {
      if (fArray[i]) ++cnt;
    }

    return cnt;
  }

  const T* CData() { return fArray; }

  operator bool() { return !Empty(); }

 private:
  T fArray[N] = {nullptr};
};

template <typename ValueType>
auto make_array(ValueType val) {
  return CFArray<ValueType, ARRAY_SIZE(val)>{val};
}
}  // namespace CF
