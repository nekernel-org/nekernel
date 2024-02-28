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
STATIC Register64 kRegGdt;

void GDTLoader::Load(Register64 &gdt) {
  kRegGdt.Base = gdt.Base;
  kRegGdt.Limit = gdt.Limit;

  rt_load_gdt(kRegGdt);
}

STATIC HAL::Register64 kRegIdt;

void IDTLoader::Load(Register64 &idt) {
  UInt8 a1, a2;

  a1 = HAL::In8(0x21);  // save masks
  a2 = HAL::In8(0xA1);

  // Remap PIC.
  HAL::Out8(0x20, 0x11);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA0, 0x11);
  HAL::rt_wait_400ns();
  HAL::Out8(0x21, 0x20);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA1, 0x28);
  HAL::rt_wait_400ns();
  HAL::Out8(0x21, 0x04);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA1, 0x02);
  HAL::rt_wait_400ns();
  HAL::Out8(0x21, 0x01);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA1, 0x01);
  HAL::rt_wait_400ns();
  HAL::Out8(0x21, a1);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA1, a2);

  volatile ::HCore::UIntPtr *baseIdt = (::HCore::UIntPtr *)idt.Base;

  MUST_PASS(baseIdt[0]);

  ::HCore::Detail::AMD64::InterruptDescriptorAMD64 *kInterruptVectorTable =
      new ::HCore::Detail::AMD64::InterruptDescriptorAMD64[kKernelIdtSize];

  for (auto i = 0; i < kKernelIdtSize; i++) {
    kInterruptVectorTable[i].Selector = kGdtCodeSelector;
    kInterruptVectorTable[i].Ist = 0x0;
    kInterruptVectorTable[i].TypeAttributes = kInterruptGate;
    kInterruptVectorTable[i].OffsetLow = (baseIdt[i] & 0xFF);
    kInterruptVectorTable[i].OffsetMid = ((baseIdt[i] & 0xFFFF) >> 16);
    kInterruptVectorTable[i].OffsetHigh = ((baseIdt[i] & 0xFFFFFFFF) >> 32);
    kInterruptVectorTable[i].Zero = 0x0;
  }

  kRegIdt.Base = (UIntPtr)&kInterruptVectorTable[0];
  kRegIdt.Limit = sizeof(::HCore::Detail::AMD64::InterruptDescriptorAMD64) *
                  kKernelIdtSize -
              1;

  rt_load_idt(kRegIdt);
}

void GDTLoader::Load(Ref<Register64> &gdt) { GDTLoader::Load(gdt.Leak()); }

void IDTLoader::Load(Ref<Register64> &idt) { IDTLoader::Load(idt.Leak()); }
}  // namespace HCore::HAL
