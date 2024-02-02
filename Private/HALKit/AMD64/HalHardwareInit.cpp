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

static HCore::HAL::Register64* kIdtRegister;
static HCore::HAL::Register64* kGdtRegister;

namespace HCore {
bool ke_init_hal() {
  kIdtRegister = nullptr;
  kGdtRegister = nullptr;

  kIdtRegister = new HCore::HAL::Register64();
  kIdtRegister->Base = (UIntPtr)__EXEC_IVT;
  kIdtRegister->Limit = sizeof(HAL::Register64) * 256;

  HAL::IDTLoader idt;
  idt.Load(*kIdtRegister);

  return true;
}
}  // namespace HCore
