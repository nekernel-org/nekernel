/* -------------------------------------------

    Copyright Mahrouss Logic

    File: Prcoessor.hxx
    Purpose: AMD processor abstraction.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

#pragma once

#include <NewKit/Array.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Utils.hpp>

#ifdef kCPUBackendName
#undef kCPUBackendName
#endif  // ifdef kCPUBackendName

#define kCPUBackendName "AMD64"

#define IsActiveLow(flag) (flag & 2)
#define IsLevelTriggered(flag) (flag & 8)

#define kInterruptGate 0x8E
#define kTrapGate 0xEF
#define kTaskGate 0b10001100
#define kGdtCodeSelector 0x08

namespace HCore {
namespace Detail::AMD64 {
struct PACKED InterruptDescriptorAMD64 final {
  UInt16 OffsetLow;  // offset bits 0..15
  UInt16 Selector;   // a code segment selector in GDT or LDT
  UInt8  Ist;  // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
  UInt8 TypeAttributes;  // gate type, dpl, and p fields
  UInt16 OffsetMid;      // offset bits 16..31
  UInt32 OffsetHigh;     // offset bits 32..63
  UInt32 Zero;           // reserved
};
}  // namespace Detail::AMD64
} // namespace HCore

namespace HCore::HAL {
extern "C" UChar In8(UInt16 port);
extern "C" UShort In16(UInt16 port);
extern "C" UInt In32(UInt16 port);

extern "C" void Out16(UShort port, UShort byte);
extern "C" void Out8(UShort port, UChar byte);
extern "C" void Out32(UShort port, UInt byte);

extern "C" void rt_wait_for_io();
extern "C" void rt_halt();
extern "C" void rt_cli();
extern "C" void rt_sti();
extern "C" void rt_cld();

class PACKED Register64 {
 public:
  UShort Limit;
  UIntPtr Base;
};

using RawRegister = UInt64;

using InterruptId = UShort; /* For each element in the IVT */
using interruptTrap = UIntPtr(UIntPtr sp);

typedef UIntPtr Reg;

struct PACKED StackFrame {
  Reg ExceptionZ;
  Reg IntNum;
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

extern "C" void rt_load_idt(HCore::HAL::Register64 ptr);
extern "C" void rt_load_gdt(HCore::HAL::Register64 ptr);

#define kKernelIdtSize 256