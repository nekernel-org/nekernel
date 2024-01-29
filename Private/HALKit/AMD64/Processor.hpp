/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>

#define IsActiveLow(flag) (flag & 2)
#define IsLevelTriggered(flag) (flag & 8)

namespace hCore::HAL {
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
  Reg R15;  // Reserved: Multi Processor manager (Hal)
  Reg SID;  // Reserved: system call id (Hal)
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
  UIntPtr Base;
  UIntPtr BaseMiddle;
  UIntPtr BaseHigh;

  UShort Limit;
  UChar Gran;
  UChar AB;

  operator bool() { return Base > Limit; }
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
}  // namespace hCore::HAL

extern "C" void idt_handle_system_call(hCore::UIntPtr rsp);
extern "C" void idt_handle_generic(hCore::UIntPtr rsp);
extern "C" void load_idt(hCore::voidPtr ptr);
extern "C" void load_gdt(hCore::voidPtr ptr);
extern "C" void idt_handle_gpf(hCore::UIntPtr rsp);
extern "C" void idt_handle_math(hCore::UIntPtr rsp);
extern "C" void idt_handle_pf(hCore::UIntPtr rsp);
