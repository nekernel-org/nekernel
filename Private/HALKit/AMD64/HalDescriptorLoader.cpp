/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

namespace HCore::HAL {
STATIC RegisterGDT kRegGdt;

void GDTLoader::Load(RegisterGDT &gdt) {
  MUST_PASS(gdt.Base != 0);

  kRegGdt.Base = gdt.Base;
  kRegGdt.Limit = gdt.Limit;

  rt_load_gdt(kRegGdt);
}

STATIC HAL::Register64 kRegIdt;

STATIC ::HCore::Detail::AMD64::InterruptDescriptorAMD64
    kInterruptVectorTable[kKernelIdtSize];

void IDTLoader::Load(Register64 &idt) {
  volatile ::HCore::UIntPtr **baseIdt = (volatile ::HCore::UIntPtr **)idt.Base;
  MUST_PASS(baseIdt);

  MUST_PASS(baseIdt[0]);

  for (UInt16 i = 0; i < kKernelIdtSize; i++) {
    kInterruptVectorTable[i].Selector = kGdtCodeSelector;
    kInterruptVectorTable[i].Ist = 0x0;
    kInterruptVectorTable[i].TypeAttributes = kInterruptGate;
    kInterruptVectorTable[i].OffsetLow = ((UIntPtr)baseIdt[i] & 0xFFFF);
    kInterruptVectorTable[i].OffsetMid = (((UIntPtr)baseIdt[i] >> 16) & 0xFFFF);
    kInterruptVectorTable[i].OffsetHigh =
        (((UIntPtr)baseIdt[i] >> 32) & 0xFFFFFFFF);
    kInterruptVectorTable[i].Zero = 0x0;
  }

  kRegIdt.Base = (UIntPtr)kInterruptVectorTable;
  kRegIdt.Limit = sizeof(::HCore::Detail::AMD64::InterruptDescriptorAMD64) *
                      kKernelIdtSize -
                  1;

  rt_load_idt(kRegIdt);

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
}

void GDTLoader::Load(Ref<RegisterGDT> &gdt) { GDTLoader::Load(gdt.Leak()); }

void IDTLoader::Load(Ref<Register64> &idt) { IDTLoader::Load(idt.Leak()); }
}  // namespace HCore::HAL
