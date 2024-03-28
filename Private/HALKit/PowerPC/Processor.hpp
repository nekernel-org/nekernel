/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>

#define __aligned __attribute__((aligned(4)))

namespace NewOS::HAL {
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
}  // namespace NewOS::HAL

extern "C" void int_handle_math(NewOS::UIntPtr sp);
extern "C" void int_handle_pf(NewOS::UIntPtr sp);
extern "C" void* __ppc_alloca(size_t sz);
