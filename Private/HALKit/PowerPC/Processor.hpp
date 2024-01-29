/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>

#define __aligned __attribute__((aligned(4)))

namespace hCore::HAL {
typedef UIntPtr Reg;

struct __aligned StackFrame {
  Reg R0;
  Reg R1;
  Reg R2;
  Reg R3;
  Reg R4;
  Reg R5;
  Reg R6;
  Reg R7;
  Reg ID;  // R8
};

typedef StackFrame* StackFramePtr;

inline void rt_halt() {
  while (1) {
  }
}

inline void rt_cli() {}
}  // namespace hCore::HAL

extern "C" void int_handle_math(hCore::UIntPtr sp);
extern "C" void int_handle_pf(hCore::UIntPtr sp);
extern "C" void* __ppc_alloca(size_t sz);
