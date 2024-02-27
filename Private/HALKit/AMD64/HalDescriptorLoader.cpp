/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>

EXTERN_C HCore::UIntPtr ke_handle_irq;

namespace HCore::HAL {
static Register64 kRegGdt;

void GDTLoader::Load(Register64 &gdt) {
  kRegGdt.Base = gdt.Base;
  kRegGdt.Limit = gdt.Limit;

  rt_load_gdt(kRegGdt);
}

static HAL::Register64 kRegIdt;

void IDTLoader::Load(Register64 &idt) {
  volatile ::HCore::Detail::AMD64::InterruptDescriptorAMD64* baseIdt = (::HCore::Detail::AMD64::InterruptDescriptorAMD64 *)idt.Base;

  for (auto i = 0; i < kKernelIdtSize; i++) {
    baseIdt[i].Selector = kGdtCodeSelector;
    baseIdt[i].Ist = 00;
    baseIdt[i].TypeAttributes = kInterruptGate;
    baseIdt[i].OffsetLow = (UInt16)((UInt32) ke_handle_irq & 0x000000000000ffff);
    baseIdt[i].OffsetMid = (UInt16)(((UInt32) ke_handle_irq & 0x00000000ffff0000) >> 16);
    baseIdt[i].OffsetHigh = (UInt32)((UInt32) ke_handle_irq & 0xffffffff00000000) >> 32;
    baseIdt[i].Zero = 0;
  }

  rt_load_idt(idt);
}

void GDTLoader::Load(Ref<Register64> &gdt) { GDTLoader::Load(gdt.Leak()); }

void IDTLoader::Load(Ref<Register64> &idt) { IDTLoader::Load(idt.Leak()); }
}  // namespace HCore::HAL
