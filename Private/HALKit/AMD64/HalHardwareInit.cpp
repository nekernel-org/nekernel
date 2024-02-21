/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>

// bugs = 0

extern "C" HCore::VoidPtr __EXEC_IVT;

namespace HCore {
bool ke_init_hal() {
  HCore::HAL::Register64 kIdtRegister;

  kIdtRegister.Base = (UIntPtr)__EXEC_IVT;
  kIdtRegister.Limit = sizeof(HAL::Register64) * 255;

  HAL::IDTLoader idt;
  idt.Load(kIdtRegister);

  return true;
}
}  // namespace HCore
