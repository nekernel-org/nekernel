/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <CoreFoundation.fwrk/headers/Foundation.h>

namespace CF {
template <typename T>
class CFAtom final {
 public:
  explicit CFAtom() = default;
  ~CFAtom()         = default;

 public:
  CFAtom& operator=(const CFAtom&) = delete;
  CFAtom(const CFAtom&)            = delete;

 public:
  T operator[](SizeT bit) { return (fArrayOfAtoms & (1 << bit)); }

  void operator|(SizeT bit) { fArrayOfAtoms |= (1 << bit); }

  friend Boolean operator==(CFAtom<T>& atomic, const T& idx) { return atomic[idx] == idx; }

  friend Boolean operator!=(CFAtom<T>& atomic, const T& idx) { return atomic[idx] == idx; }

 private:
  T fArrayOfAtoms;
};
}  // namespace CF
