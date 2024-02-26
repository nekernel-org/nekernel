/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>

namespace HCore::HAL {
void GDTLoader::Load(Register64 &gdt) {
  Register64 gdtReg;

  gdtReg.Base = gdt.Base;
  gdtReg.Limit = gdt.Limit;

  rt_load_gdt(gdtReg);
}

namespace Detail::AMD64 {
struct InterruptDescriptorAMD64 final {
  UInt16 OffsetLow;  // offset bits 0..15
  UInt16 Selector;   // a code segment selector in GDT or LDT
  UInt8
      Ist;  // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
  UInt8 TypeAttributes;  // gate type, dpl, and p fields
  UInt16 OffsetMid;      // offset bits 16..31
  UInt32 OffsetHigh;     // offset bits 32..63
  UInt32 Zero;           // reserved
};
}  // namespace Detail::AMD64

#define kInterruptGate 0x8E
#define kTrapGate 0x8F
#define kTaskGate 0x85
#define kGdtSelector 0x08

extern "C" HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr &rsp);

static ALIGN(0x10)
    Detail::AMD64::InterruptDescriptorAMD64 kIdtRegs[kKernelMaxSystemCalls];

static HAL::Register64 kRegIdt;

void IDTLoader::Load(Register64 &idt) {
  VoidPtr *baseIdt = (VoidPtr *)idt.Base;

  for (auto i = 0; i < 32; i++) {
    kIdtRegs[i].Selector = kGdtSelector;
    kIdtRegs[i].Ist = 001;
    kIdtRegs[i].TypeAttributes = kTrapGate;
    kIdtRegs[i].OffsetLow = (UIntPtr)baseIdt & 0xFFFF;
    kIdtRegs[i].OffsetMid = (UIntPtr)baseIdt >> 16 & 0xFFFF;
    kIdtRegs[i].OffsetHigh = (UIntPtr)baseIdt >> 32 & 0xFFFFFFFF;
    kIdtRegs[i].Zero = 0;
  }

  kRegIdt.Base = (UIntPtr)&kIdtRegs[0];
  kRegIdt.Limit =
      sizeof(Detail::AMD64::InterruptDescriptorAMD64) * idt.Limit - 1;

  rt_load_idt(kRegIdt);
}

void GDTLoader::Load(Ref<Register64> &gdt) { GDTLoader::Load(gdt.Leak()); }

void IDTLoader::Load(Ref<Register64> &idt) { IDTLoader::Load(idt.Leak()); }
}  // namespace HCore::HAL
