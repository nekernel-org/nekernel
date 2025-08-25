/* -------------------------------------------

  Copyright (C) 2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NeKit/KernelPanic.h>

/// @brief Kernel Test Framework.
/// @file KernelTest.h

#define KT_TEST_VERSION_BCD (0x0002)
#define KT_TEST_VERSION "v0.0.2-kerneltest"

#define KT_TEST_FAILURE (1)

#define KT_TEST_SUCCESS (0)

#define KT_DECL_TEST(NAME, FN)                                             \
  class KT_##NAME final {                                                  \
   public:                                                                 \
    Kernel::Void        Run();                                             \
    const Kernel::Char* ToString();                                        \
  };                                                                       \
  inline Kernel::Void        KT_##NAME::Run() { MUST_PASS(FN() == true); } \
  inline const Kernel::Char* KT_##NAME::ToString() { return #FN; }

KT_DECL_TEST(ALWAYS_BREAK, []() -> bool { return false; });
KT_DECL_TEST(ALWAYS_GOOD, []() -> bool { return true; });