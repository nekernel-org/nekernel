/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>

namespace HCore::HAL {
namespace Detail {
STATIC RegisterGDT kRegGdt;
STATIC HAL::Register64 kRegIdt;

STATIC ::HCore::Detail::AMD64::InterruptDescriptorAMD64
    kInterruptVectorTable[kKernelIdtSize];

STATIC Void RemapPIC(Void) noexcept {
  UInt8 a1, a2;

  a1 = HAL::In8(0x21);  // save masks
  a2 = HAL::In8(0xA1);

  // Remap PIC.
  HAL::Out8(0x20, 0x10 | 0x01);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA0, 0x10 | 0x01);
  HAL::rt_wait_400ns();

  HAL::Out8(0x21, 0x28);
  HAL::rt_wait_400ns();
  HAL::Out8(0xA1, 0x30);

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
} // namespace Detail

/// @brief Loads the provided Global Descriptor Table.
/// @param gdt 
/// @return 
Void GDTLoader::Load(RegisterGDT &gdt) {
  MUST_PASS(gdt.Base != 0);

  Detail::kRegGdt.Base = gdt.Base;
  Detail::kRegGdt.Limit = gdt.Limit;

  hal_load_gdt(Detail::kRegGdt);
}

Void IDTLoader::Load(Register64 &idt) {
  volatile ::HCore::UIntPtr **baseIdt = (volatile ::HCore::UIntPtr **)idt.Base;

  MUST_PASS(baseIdt);

  for (UInt16 i = 0; i < kKernelIdtSize; i++) {
    MUST_PASS(baseIdt[i]);

    Detail::kInterruptVectorTable[i].Selector = kGdtCodeSelector;
    Detail::kInterruptVectorTable[i].Ist = 0x0;
    Detail::kInterruptVectorTable[i].TypeAttributes = kInterruptGate;
    Detail::kInterruptVectorTable[i].OffsetLow = ((UIntPtr)baseIdt[i] & 0xFFFF);
    Detail::kInterruptVectorTable[i].OffsetMid = (((UIntPtr)baseIdt[i] >> 16) & 0xFFFF);
    Detail::kInterruptVectorTable[i].OffsetHigh =
        (((UIntPtr)baseIdt[i] >> 32) & 0xFFFFFFFF);
    Detail::kInterruptVectorTable[i].Zero = 0x0;
  }

  Detail::kRegIdt.Base = reinterpret_cast<UIntPtr>(Detail::kInterruptVectorTable);
  Detail::kRegIdt.Limit = sizeof(::HCore::Detail::AMD64::InterruptDescriptorAMD64) *
                  (kKernelIdtSize - 1);

  hal_load_idt(Detail::kRegIdt);

  Detail::RemapPIC();
}

void GDTLoader::Load(Ref<RegisterGDT> &gdt) { GDTLoader::Load(gdt.Leak()); }

void IDTLoader::Load(Ref<Register64> &idt) { IDTLoader::Load(idt.Leak()); }
}  // namespace HCore::HAL
