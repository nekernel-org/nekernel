/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>

#define IsActiveLow(flag) (flag & 2)
#define IsLevelTriggered(flag) (flag & 8)

namespace HCore::HAL {
extern "C" UChar in8(UInt16 port);
extern "C" UShort in16(UInt16 port);
extern "C" UInt in32(UInt16 port);

extern "C" void out16(UShort port, UShort byte);
extern "C" void out8(UShort port, UChar byte);
extern "C" void out32(UShort port, UInt byte);

extern "C" void rt_wait_for_io();
extern "C" void rt_halt();
extern "C" void rt_cli();
extern "C" void rt_sti();
extern "C" void rt_cld();

class Register64 {
 public:
  UIntPtr Base;
  UShort Limit;

  operator bool() { return Base > Limit; }
};

using RawRegister = UInt64;

using InterruptId = UShort; /* For each element in the IVT */
using interruptTrap = UIntPtr(UIntPtr sp);

typedef UIntPtr Reg;

struct __attribute__((packed)) StackFrame {
  Reg Rax;
  Reg Rbx;
  Reg Rcx;
  Reg Rdx;
  Reg Rsi;
  Reg Rdi;
  Reg Rbp;
  Reg Rsp;
  Reg R8;
  Reg R9;
  Reg R10;
  Reg R11;
  Reg R12;
  Reg R13;
  Reg R14;
  Reg R15;
};

typedef StackFrame *StackFramePtr;

class InterruptDescriptor final {
 public:
  UShort Offset;
  UShort Selector;
  UChar Ist;
  UChar Atrributes;

  UShort SecondOffset;
  UInt ThirdOffset;
  UInt Zero;

  operator bool() { return Offset != 0xFFFF; }
};

using InterruptDescriptorArray = Array<InterruptDescriptor, 256>;

class SegmentDescriptor final {
 public:
  UInt16 Base;
  UInt8 BaseMiddle;
  UInt8 BaseHigh;

  UShort Limit;
  UChar Gran;
  UChar AccessByte;
};

/***
 * @brief Segment Boolean operations
 */
class SegmentDescriptorComparator final {
 public:
  bool IsValid(SegmentDescriptor &seg) { return seg.Base > seg.Limit; }
  bool Equals(SegmentDescriptor &seg, SegmentDescriptor &segRight) {
    return seg.Base == segRight.Base && seg.Limit == segRight.Limit;
  }
};

using SegmentArray = Array<SegmentDescriptor, 6>;

class GDTLoader final {
 public:
  static void Load(Register64 &gdt);
  static void Load(Ref<Register64> &gdt);
};

class IDTLoader final {
 public:
  static void Load(Register64 &idt);
  static void Load(Ref<Register64> &idt);
};

void system_get_cores(voidPtr rsdPtr);
}  // namespace HCore::HAL

extern "C" void idt_handle_system_call(HCore::UIntPtr rsp);
extern "C" void idt_handle_generic(HCore::UIntPtr rsp);
extern "C" void idt_handle_gpf(HCore::UIntPtr rsp);
extern "C" void idt_handle_math(HCore::UIntPtr rsp);
extern "C" void idt_handle_pf(HCore::UIntPtr rsp);

extern "C" void rt_load_idt(HCore::voidPtr ptr);
extern "C" void rt_load_gdt(HCore::voidPtr ptr);
