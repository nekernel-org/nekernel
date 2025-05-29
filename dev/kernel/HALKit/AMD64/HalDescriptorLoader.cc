/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <ArchKit/ArchKit.h>
#include <FSKit/NeFS.h>
#include <HALKit/AMD64/Processor.h>

namespace Kernel::HAL {
namespace Detail {
  STATIC ::Kernel::Detail::AMD64::InterruptDescriptorAMD64 kInterruptVectorTable[kKernelIdtSize] =
      {};
}  // namespace Detail

/// @brief Loads the provided Global Descriptor Table.
/// @param gdt
/// @return
Void GDTLoader::Load(Register64& gdt) {
#ifndef __NE_MODULAR_KERNEL_COMPONENTS__
  hal_load_gdt(gdt);
#endif  // __NE_MODULAR_KERNEL_COMPONENTS__
}

Void IDTLoader::Load(Register64& idt) {
#ifndef __NE_MODULAR_KERNEL_COMPONENTS__
  rt_cli();

  volatile UIntPtr** ptr_ivt = (volatile UIntPtr**) idt.Base;

  for (SizeT idt_indx = 0; idt_indx < kKernelIdtSize; ++idt_indx) {
    Detail::kInterruptVectorTable[idt_indx].Selector       = kIDTSelector;
    Detail::kInterruptVectorTable[idt_indx].Ist            = 0;
    Detail::kInterruptVectorTable[idt_indx].TypeAttributes = kInterruptGate;
    Detail::kInterruptVectorTable[idt_indx].OffsetLow      = ((UIntPtr) ptr_ivt[idt_indx] & 0xFFFF);
    Detail::kInterruptVectorTable[idt_indx].OffsetMid =
        (((UIntPtr) ptr_ivt[idt_indx] >> 16) & 0xFFFF);
    Detail::kInterruptVectorTable[idt_indx].OffsetHigh =
        (((UIntPtr) ptr_ivt[idt_indx] >> 32) & 0xFFFFFFFF);

    Detail::kInterruptVectorTable[idt_indx].Zero = 0;
  }

  idt.Base  = (UIntPtr) &Detail::kInterruptVectorTable[0];
  idt.Limit = sizeof(::Kernel::Detail::AMD64::InterruptDescriptorAMD64) * (kKernelIdtSize);

  hal_load_idt(idt);
  rt_sti();
#endif  // __NE_MODULAR_KERNEL_COMPONENTS__

  return;
}

/// @brief Loads the Global Descriptor Table into the CPU.
/// @param gdt GDT register wrapped in a ref.
void GDTLoader::Load(Ref<Register64>& gdt) {
  if (!gdt) return;

  GDTLoader::Load(gdt.Leak());
}

/// @brief Loads the IDT, for interupts.
/// @param idt IDT register wrapped in a ref.
void IDTLoader::Load(Ref<Register64>& idt) {
  if (!idt) return;

  IDTLoader::Load(idt.Leak());
}
}  // namespace Kernel::HAL
