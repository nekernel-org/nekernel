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

#define kInterruptGate 0xE
#define kTrapGate 0xF
#define kRing0 (0 << 5)
#define kRing3 (3 << 5)
#define kIdtPresent (1 << 7)
#define kGdtSelector 0x08

extern "C" HCore::UIntPtr rt_handle_interrupts(HCore::UIntPtr &rsp);

static ALIGN(0x10)
    Detail::AMD64::InterruptDescriptorAMD64 kIdtRegs[kKernelMaxSystemCalls];
static HAL::Register64 kRegIdt;

void IDTLoader::Load(Register64 &idt) {
  for (auto i = 0; i < 32; i++) {
    kIdtRegs[i].Selector = kGdtSelector;
    kIdtRegs[i].Ist = 0x00;
    kIdtRegs[i].TypeAttributes =
        kInterruptGate | kTrapGate | kRing0 | kIdtPresent;
    kIdtRegs[i].OffsetLow = (((UIntPtr *)idt.Base)[i]) & 0xFFFF;
    kIdtRegs[i].OffsetMid = (((UIntPtr *)idt.Base)[i] >> 16) & 0xFFFF;
    kIdtRegs[i].OffsetHigh = (((UIntPtr *)idt.Base)[i] >> 32) & 0xFFFF;
    kIdtRegs[i].Zero = 0;
  }

  kIdtRegs[0x21].Selector = kGdtSelector;
  kIdtRegs[0x21].Ist = 0x00;
  kIdtRegs[0x21].TypeAttributes = kInterruptGate | kRing3 | kIdtPresent;
  kIdtRegs[0x21].OffsetLow = (((UIntPtr *)idt.Base)[0x21]) & 0xFFFF;
  kIdtRegs[0x21].OffsetMid = (((UIntPtr *)idt.Base)[0x21] >> 16) & 0xFFFF;
  kIdtRegs[0x21].OffsetHigh = (((UIntPtr *)idt.Base)[0x21] >> 32) & 0xFFFF;
  kIdtRegs[0x21].Zero = 0;

  kRegIdt.Base = (UIntPtr)kIdtRegs;
  kRegIdt.Limit = sizeof(Detail::AMD64::InterruptDescriptorAMD64) * idt.Limit;

  kcout << "HCoreKrnl: Installing Interrupt vector...\n";

  rt_load_idt(kRegIdt);

  asm volatile("sti");

  kcout << "HCoreKrnl: Interrupt Vector installed.\n";
}

void GDTLoader::Load(Ref<Register64> &gdt) { GDTLoader::Load(gdt.Leak()); }

void IDTLoader::Load(Ref<Register64> &idt) { IDTLoader::Load(idt.Leak()); }
}  // namespace HCore::HAL
