/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */
#pragma once

#include <NewKit/Defines.h>

namespace Kernel {
template <typename T>
class Atom final {
 public:
  explicit Atom() = default;
  ~Atom()         = default;

 public:
  Atom& operator=(const Atom&) = delete;
  Atom(const Atom&)            = delete;

 public:
  T operator[](Size bit) { return (fArrayOfAtoms & (1 << bit)); }

  void operator|(Size bit) { fArrayOfAtoms |= (1 << bit); }

  friend Boolean operator==(Atom<T>& atomic, const T& idx) { return atomic[idx] == idx; }

  friend Boolean operator!=(Atom<T>& atomic, const T& idx) { return atomic[idx] == idx; }

 private:
  T fArrayOfAtoms;
};
}  // namespace Kernel
