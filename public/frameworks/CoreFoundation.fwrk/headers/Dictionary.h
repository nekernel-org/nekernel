/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

namespace CF {
template <typename Key, typename Value>
class CFDictionary final {
 public:
  explicit CFDictionary() = default;
  ~CFDictionary()         = default;

  CFDictionary& operator=(const CFDictionary&) = default;
  CFDictionary(const CFDictionary&)            = default;

  Value& operator[](Key& at) {
    MUST_PASS(this->Find(at));
    return fArray[at];
  }

  Bool Empty() { return this->fCount > 0; }

  Bool Find(Key& key) { return No; }

  operator bool() { return !this->Empty(); }

 private:
  SizeT fCount{0UL};
};

template <typename KeyType, typename ValueType>
inline auto make_dict() {
  return CFDictionary<KeyType, ValueType>{};
}
}  // namespace CF
