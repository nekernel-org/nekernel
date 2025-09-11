/* -------------------------------------------

   Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

   File: Verify.h
   Purpose: System Call Interface Verification Layer.

   ------------------------------------------- */

#pragma once

#include <libSystem/SystemKit/System.h>

namespace LibSystem::Detail {
/// @author 0xf00sec, and Amlal El Mahrouss
/// @brief safe cast operator.
template <typename T, typename R = VoidPtr>
static R sys_safe_cast(const T* ptr) {
  _rtl_assert(ptr, "safe cast failed!");
  return static_cast<R>(const_cast<T*>(ptr));
}

template <typename T, typename U>
struct must_cast_traits {
  constexpr static BOOL value = false;
};

template <typename T>
struct must_cast_traits<T, T> {
  constexpr static BOOL value = true;
};

/// @author Amlal El Mahrouss
/// @brief Safe constexpr cast.
template <typename T, typename R>
constexpr R* sys_constexpr_cast(T* ptr) {
  static_assert(must_cast_traits<T, R>::value, "constexpr cast failed! types are a mismatch!");
  return static_cast<R*>(ptr);
}
}  // namespace LibSystem::Detail
